

#include "asyncrenderthread.h"
#include "asyncrenderinternal.h"
#include "asyncrenderwidget.h"
#include "geometry.h"
#include "peelrenderer.h"
#include "globaldepthpeelingsetting.h"

#include <QGLFramebufferObject>

#define glewGetContext() glewContext

using std::deque;
using std::vector;
using std::make_pair;

using namespace AsyncRenderInternal;

namespace {
void gl_errors(std::string const &where) {
	GLuint err;
	while ((err = glGetError()) != GL_NO_ERROR) {
	std::cerr << "(in " << where << ") OpenGL error #" << err
	          << ": " << gluErrorString(err) << std::endl;
	}
}
}

RenderThread::RenderThread(Controller *_controller) : controller(_controller), widget(NULL) {
	widget = new QGLWidget();
	widget->doneCurrent(); //make sure this widget's context isn't current!
}

RenderThread::~RenderThread() {
	delete widget;
	widget = NULL;
}

const Vector3f bgColor = make_vector(0.8f, 0.8f, 0.8f);

void RenderThread::run() {
	assert(widget->context());
	assert(widget->context()->isValid());
	widget->makeCurrent();

	//-----------------------------------------------
	//Init glew for this thread (needed for peeling):
	GLEWContext *glewContext = new GLEWContext;
	GLenum err = glewInit();
	PeelRenderer *peelRenderer = NULL;
	if (err != GLEW_OK) 
	{
		std::cerr << "WARNING: Failure initializing glew: " << glewGetErrorString(err) << std::endl;
		std::cerr << " ... we will continue, but code that uses extensions will cause a crash" << std::endl;
	} 
	else 
	{
		try 
		{
			peelRenderer = new PeelRenderer(glewContext);
		} 
		catch (...) 
		{
			std::cerr << "Caught exception constructing peelRenderer, will fall back to regular rendering." << std::endl;
			peelRenderer = NULL;
		}
	}

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	
	gl_errors("RenderThread setup");

	controller->renderQueueLock.lock();
	while (!controller->quitThreads) 
	{
		if (controller->renderQueue.empty()) 
		{
			controller->renderQueueHasData.wait(&controller->renderQueueLock);
			continue;
		}

		//pull job off the queue:
		Job *job = controller->renderQueue.front();
		controller->renderQueue.pop_front();

		controller->renderQueueLock.unlock();

		//shouldn't change if it's a per-thread context, which I've been lead to suspect is true.
		assert(QGLContext::currentContext() == widget->context()); 

		//TODO: could cache fb; not clear it's more efficient.
		QGLFramebufferObject fb(job->camera.size.x, job->camera.size.y, QGLFramebufferObject::Depth);
		fb.bind();
		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0,0,job->camera.size.x, job->camera.size.y);

		setupCamera(job->camera);
		if (peelRenderer && GlobalDepthPeelingSetting::enabled()) 
			peelRenderer->render(bgColor, *job->geometry);
		else 
			simpleRender(*job->geometry);

		glPopAttrib();
		fb.release();

		assert(!job->result);

		job->result = new QImage(fb.toImage());

		//pass job back to main thread:
		emit jobFinished(job);

		controller->renderQueueLock.lock();
	}
	controller->renderQueueLock.unlock();

	delete peelRenderer;
	peelRenderer = NULL;

	delete glewContext;
	glewContext = NULL;

	widget->doneCurrent();

	std::cout << "Quitting a render thread." << std::endl;
}

void RenderThread::setupCamera(Camera const &camera) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (camera.isPerspective) {
		gluPerspective(45.0, camera.size.x / float(camera.size.y), 0.01, 100.0);
	} else {
		float a = camera.size.y / float(camera.size.x);
		float s = 2.2f / length(camera.eye - camera.lookAt);
		glScalef(a * s, s, -0.01f);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera.eye.x, camera.eye.y, camera.eye.z,
		camera.lookAt.x, camera.lookAt.y, camera.lookAt.z,
		camera.up.x, camera.up.y, camera.up.z);
	gl_errors("RenderThread::setupCamera");
}

void RenderThread::simpleRender(Geometry const &geometry) 
{
	glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//Check that Vertex and Triangle have proper size:
	assert(sizeof(Vertex) == sizeof(GLfloat) * (3 + 3));
	assert(sizeof(Triangle) == sizeof(GLuint) * 3);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &(geometry.vertices[0].position));
	glNormalPointer(GL_FLOAT, sizeof(Vertex), &(geometry.vertices[0].normal));
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	// make a pass on mandatory transparent things, drawing them without culling/depth testing
	// this doesn't do much except fake the glass/air interface
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (std::vector< Group >::const_iterator g = geometry.groups.begin(); g != geometry.groups.end(); ++g)
	{
		Color c = g->color;
		if (g->ensureVisible)
			glColor4f(c.r, c.g, c.b, 0.1);
		else
			continue;
		glDrawElements(GL_TRIANGLES, g->triangle_size * 3,
			GL_UNSIGNED_INT, &(geometry.triangles[g->triangle_begin].v1));
	}

	// make a pass on opaque things, round pretty opaque things up to no transparency
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	for (std::vector< Group >::const_iterator g = geometry.groups.begin(); g != geometry.groups.end(); ++g)
	{
		Color c = g->color;
		if (c.a > 0.1)
			glColor4f(c.r, c.g, c.b, 1.0);
		else
			continue;
		glDrawElements(GL_TRIANGLES, g->triangle_size * 3,
			GL_UNSIGNED_INT, &(geometry.triangles[g->triangle_begin].v1));
	}
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	gl_errors("RenderThread::simpleRender");
}
