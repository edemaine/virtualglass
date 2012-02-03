#include "asyncrenderwidget.h"
#include "asyncrenderinternal.h"
#include "geometry.h"
#include <QtOpenGL>

#include <deque>
#include <vector>
#include <tr1/unordered_map>

using std::deque;
using std::vector;
using std::tr1::unordered_map;
using std::make_pair;

using namespace AsyncRenderInternal;

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

void Controller::queue(AsyncRenderWidget *widget, Camera const &camera, RenderData *data) {
	assert(widget->id != 0);
	Job *job = new Job(widget->id, camera, data);
	computeQueueLock.lock();
	computeQueue.push_back(job);
	computeQueueLock.unlock();
	computeQueueHasData.wakeOne();
}

void Controller::queue(AsyncRenderWidget *widget, Camera const &camera, Geometry *geometry) {
	Job *job = new Job(widget->id, camera, NULL, geometry);
	renderQueueLock.lock();
	renderQueue.push_back(job);
	renderQueueLock.unlock();
	renderQueueHasData.wakeOne();
}

void Controller::jobFinished(Job *job) {
	assert(job);
	unordered_map< uint32_t, AsyncRenderWidget * >::iterator f = idToWidget.find(job->requesterId);
	if (f == idToWidget.end()) {
		delete job->data;
		job->data = NULL;
		delete job->geometry;
		job->geometry = NULL;
		delete job->result;
		job->result = NULL;
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
		while (controller->computeQueue.empty()) {
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
		controller->renderQueue.push_back(job);
		controller->renderQueueHasData.wakeOne();
		controller->renderQueueLock.unlock();

		controller->computeQueueLock.lock();
	}
	controller->computeQueueLock.unlock();
}


//============================================

RenderThread::RenderThread(Controller *_controller) : controller(_controller), context(NULL) {
	context = new QGLContext(QGLFormat(QGL::AlphaChannel | QGL::DepthBuffer));
	if (!context->create()) {
		throw std::string("Cannot create context.");
	}
}

RenderThread::~RenderThread() {
	delete context;
	context = NULL;
}

void RenderThread::run() {
	controller->renderQueueLock.lock();
	while (1) {
		while (controller->renderQueue.empty()) {
			controller->renderQueueHasData.wait(&controller->renderQueueLock);
		}
		if (controller->quitThreads) break;
		if (controller->renderQueue.empty()) continue;

		//pull job off the queue:
		Job *job = controller->renderQueue.front();
		controller->renderQueue.pop_front();

		controller->renderQueueLock.unlock();

		//TODO: actually run job.

		//pass job back to main thread:
		emit jobFinished(job);

		controller->renderQueueLock.lock();
	}
	controller->renderQueueLock.unlock();
}
