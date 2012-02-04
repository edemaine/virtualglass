#include "asyncrenderwidget.h"
#include "asyncrenderinternal.h"
#include "geometry.h"
#include <QtOpenGL>
#include <GL/glu.h>

#include <deque>
#include <vector>
#include <tr1/unordered_map>

using std::deque;
using std::vector;
using std::tr1::unordered_map;
using std::make_pair;

using namespace AsyncRenderInternal;

//===========================================

void Job::deleteData() {
	delete result;
	result = NULL;
	delete geometry;
	geometry = NULL;
	delete data;
	data = NULL;
}

//===========================================

Controller &Controller::controller() {
	static Controller global_controller;
	return global_controller;
}

Controller::Controller() : quitThreads(false), freshId(1) {
	//create compute thread(s):
	for (unsigned int i = 0; i < 1; ++i) {
		ComputeThread *thread = new ComputeThread(this);
		thread->start();
		threads.push_back(thread);
	}

	//create render threads:
	for (unsigned int i = 0; i < 1; ++i) {
		RenderThread *thread = new RenderThread(this);
		connect(thread, SIGNAL(jobFinished(Job*)), this, SLOT(jobFinished(Job*)));
		thread->start();
		threads.push_back(thread);
	}

}

Controller::~Controller() {

	std::cout << "Asking render and compute threads to quit." << std::endl;

	computeQueueLock.lock();
	renderQueueLock.lock();
	quitThreads = true;
	renderQueueLock.unlock();
	computeQueueLock.unlock();

	computeQueueHasData.wakeAll();
	renderQueueHasData.wakeAll();
	for (vector< QThread * >::iterator t = threads.begin(); t != threads.end(); ++t) {
		(*t)->wait();
		delete *t;
		*t = NULL;
	}

	//Don't need to lock here, as all threads have stopped:

	while (!computeQueue.empty()) {
		delete computeQueue.front();
		computeQueue.pop_front();
	}

	while (!renderQueue.empty()) {
		delete renderQueue.front();
		renderQueue.pop_front();
	}
}

void Controller::registerWidget(AsyncRenderWidget *widget) {
	//widget gets a fresh id and gets added to our map:
	assert(widget);
	assert(widget->id == 0);
	widget->id = freshId;
	assert(widget->id != 0);
	++freshId;
	assert(!idToWidget.count(widget->id));
	idToWidget.insert(make_pair(widget->id, widget));
}

void Controller::unregisterWidget(AsyncRenderWidget *widget) {
	//widget is deleted from our map:
	assert(widget);
	assert(widget->id != 0);
	unordered_map< uint32_t, AsyncRenderWidget * >::iterator f = idToWidget.find(widget->id);
	assert(f != idToWidget.end());
	idToWidget.erase(f);
}

void replaceOrAddJob(deque< Job * > &queue, Job *job) {
	bool found = false;
	for (deque< Job * >::iterator j = queue.begin(); j != queue.end(); ++j) {
		if ((*j)->requesterId == job->requesterId) {
			assert(!found);
			found = true;
			//There's already a job in the queue from the same requester, so take its place in line and clean up its data:
			(*j)->deleteData();
			delete *j;
			*j = job;
		}
	}
	if (found) {
		delete job;
	} else {
		queue.push_back(job);
	}
}

void Controller::queue(AsyncRenderWidget *widget, Camera const &camera, RenderData *data) {
	assert(widget->id != 0);
	Job *job = new Job(widget->id, camera, data);
	computeQueueLock.lock();
	replaceOrAddJob(computeQueue, job);
	computeQueueLock.unlock();
	computeQueueHasData.wakeOne();
}

void Controller::queue(AsyncRenderWidget *widget, Camera const &camera, Geometry *geometry) {
	Job *job = new Job(widget->id, camera, NULL, geometry);
	renderQueueLock.lock();
	replaceOrAddJob(renderQueue, job);
	renderQueueLock.unlock();
	renderQueueHasData.wakeOne();
}

void Controller::jobFinished(Job *job) {
	assert(job);
	unordered_map< uint32_t, AsyncRenderWidget * >::iterator f = idToWidget.find(job->requesterId);
	if (f == idToWidget.end()) {
		job->deleteData();
	} else {
		f->second->renderFinished(job->camera, job->data, job->geometry, job->result);
		job->data = NULL;
		job->geometry = NULL;
		job->result = NULL;
	}
	delete job;
}

//============================================

ComputeThread::ComputeThread(Controller *_controller) : controller(_controller) {
	assert(controller);
}

ComputeThread::~ComputeThread() {
}

void ComputeThread::run() {
	controller->computeQueueLock.lock();
	while (1) {
		while (controller->computeQueue.empty() && !controller->quitThreads) {
			controller->computeQueueHasData.wait(&controller->computeQueueLock);
		}
		if (controller->quitThreads) break;
		if (controller->computeQueue.empty()) continue;

		//pull job off the queue:
		Job *job = controller->computeQueue.front();
		controller->computeQueue.pop_front();

		controller->computeQueueLock.unlock();

		//Run geometry calculation:
		assert(job->geometry == NULL);
		assert(job->data);
		job->geometry = job->data->getGeometry();

		//Pass back to render queue:
		controller->renderQueueLock.lock();
		replaceOrAddJob(controller->renderQueue, job);
		controller->renderQueueHasData.wakeOne();
		controller->renderQueueLock.unlock();

		controller->computeQueueLock.lock();
	}
	controller->computeQueueLock.unlock();
	std::cout << "Quitting a compute thread." << std::endl;
}


//============================================

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

void RenderThread::run() {
	assert(widget->context());
	assert(widget->context()->isValid());
	widget->makeCurrent();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	//TODO: I should really set the position and color of LIGHT0, but w/e
	
	gl_errors("RenderThread setup");

	controller->renderQueueLock.lock();
	while (1) {
		while (controller->renderQueue.empty() && !controller->quitThreads) {
			controller->renderQueueHasData.wait(&controller->renderQueueLock);
		}
		if (controller->quitThreads) {
			widget->doneCurrent();
			break;
		}
		if (controller->renderQueue.empty()) continue;

		//pull job off the queue:
		Job *job = controller->renderQueue.front();
		controller->renderQueue.pop_front();

		controller->renderQueueLock.unlock();

		assert(QGLContext::currentContext() == widget->context()); //shouldn't change if it's a per-thread context, which I've been lead to suspect is true.

		//TODO: could cache fb; not clear it's more efficient.
		QGLFramebufferObject fb(job->camera.size.x, job->camera.size.y, QGLFramebufferObject::Depth);
		fb.bind();
		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0,0,job->camera.size.x, job->camera.size.y);

		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		setupCamera(job->camera);
		simpleRender(*job->geometry);

		glPopAttrib();
		fb.release();

		assert(!job->result);

		job->result = new QImage(fb.toImage());

		/*
		//Placeholder execute code:
		job->result = new QImage(job->camera.size.x, job->camera.size.y, QImage::Format_ARGB32);
		job->result->fill(QColor(rand() % 256, rand() % 256, rand() % 256, 255));
		*/


		//pass job back to main thread:
		emit jobFinished(job);

		controller->renderQueueLock.lock();
	}
	controller->renderQueueLock.unlock();
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

void RenderThread::simpleRender(Geometry const &geometry) {
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHTING);
	//Check that Vertex and Triangle have proper size:
	assert(sizeof(Vertex) == sizeof(GLfloat) * (3 + 3));
	assert(sizeof(Triangle) == sizeof(GLuint) * 3);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &(geometry.vertices[0].position));
	glNormalPointer(GL_FLOAT, sizeof(Vertex), &(geometry.vertices[0].normal));
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	for (std::vector< Group >::const_iterator g = geometry.groups.begin(); g != geometry.groups.end(); ++g) {
		assert(g->color);
		Color* c = g->color;
		glColor4f(c->r, c->g, c->b, c->a);
		glDrawElements(GL_TRIANGLES, g->triangle_size * 3,
			GL_UNSIGNED_INT, &(geometry.triangles[g->triangle_begin].v1));
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	//glDisable(GL_LIGHTING);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	gl_errors("RenderThread::simpleRender");
}
