
#include "asyncrenderthread.h"
#include "asyncrenderinternal.h"
#include "asyncrenderwidget.h"
#include "geometry.h"
#include "peelrenderer.h"
#include "globaldepthpeelingsetting.h"
#include "globalbackgroundcolor.h"
#include "constants.h"
#include "glassopengl.h"

#include <QGLFramebufferObject>

#define glewGetContext() glewContext

using std::vector;

using namespace AsyncRenderInternal;

RenderThread::RenderThread(Controller *_controller) : controller(_controller), widget(NULL) 
{
	widget = new QGLWidget(QGLFormat(QGL::AlphaChannel | QGL::DoubleBuffer | QGL::DepthBuffer));
	widget->doneCurrent(); //make sure this widget's context isn't current!
}

RenderThread::~RenderThread() 
{
	delete widget;
	widget = NULL;
}

void RenderThread::run() 
{
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

	GlassOpenGL::initialize();
	GlassOpenGL::errors("RenderThread::run()");

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

		QGLFramebufferObject fb(job->camera.size.x, job->camera.size.y, QGLFramebufferObject::Depth);
		fb.bind();
		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0, 0, job->camera.size.x, job->camera.size.y);

		setupCamera(job->camera);
		if (peelRenderer && GlobalDepthPeelingSetting::enabled()) 
			peelRenderer->render(*job->geometry);
		else 
			GlassOpenGL::renderWithoutDepthPeeling(*job->geometry);

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
}

void RenderThread::setupCamera(Camera const &camera) 
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float w = camera.size.x;
	float h = camera.size.y;

	if (camera.isPerspective) 
	{
		gluPerspective(45.0, w / h, 0.1, 100.0);
	} 
	else 
	{
		float s = 2.2f / length(camera.eye - camera.lookAt);
		glScalef(h / w * s, s, -0.01f);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camera.eye.x, camera.eye.y, camera.eye.z,
		camera.lookAt.x, camera.lookAt.y, camera.lookAt.z,
		camera.up.x, camera.up.y, camera.up.z);

	GlassOpenGL::errors("RenderThread::setupCamera");
}

