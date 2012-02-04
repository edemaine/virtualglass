#include "asyncrenderthread.h"
#include "asyncrenderwidget.h"
#include "asyncrenderinternal.h"

#include "geometry.h"
#include <qgl.h>
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
	while (!controller->quitThreads) {
		if (controller->computeQueue.empty()) {
			controller->computeQueueHasData.wait(&controller->computeQueueLock);
			continue;
		}

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
