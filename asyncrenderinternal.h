#ifndef ASYNCRENDERINTERNAL_H
#define ASYNCRENDERINTERNAL_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "glew.h"
#include <qgl.h>
#include "asyncrenderwidget.h"
#include <deque>
#include <vector>

using std::deque;
using std::vector;
using std::make_pair;

#ifdef UNORDERED_MAP_WORKAROUND
#include <tr1/unordered_map>
using std::tr1::unordered_map;
#else
#include <unordered_map>
using std::unordered_map;
#endif

class Geometry;
class QImage;

namespace AsyncRenderInternal {

//------------------------------------------
// Jobs are passed between threads to get stuff rendered.
class Job
{
	public:
		Job(uint32_t _requesterId, Camera const &_camera, RenderData *_data, Geometry *_geometry = NULL) 
			: requesterId(_requesterId), camera(_camera), data(_data), geometry(_geometry), result(NULL) {
		}

		//deletes data,geometry,result and sets 'em to NULL:
		void deleteData();
		uint32_t requesterId;
		Camera camera;
		RenderData *data;
		Geometry *geometry;
		QImage *result;
};

//------------------------------------------
// The Controller is in charge of coordinating individual
// widgets and the backend threads, using queues of jobs.

class Controller : public QObject
{
	Q_OBJECT
	friend class RenderThread;
	friend class ComputeThread;

	//there's a singleton, global render controller you get with this static member function:
	public:
		static Controller &controller();
	private:
		Controller();
		virtual ~Controller();
		std::vector< QThread * > threads; //kept around so we can wait() on 'em later.
		bool quitThreads; //how we tell threads to suicide later.

	//These functions are called by AsyncRenderWidget constructor/destructor to assign ids to active widgets:
	public:
		void registerWidget(AsyncRenderWidget *widget);
		void unregisterWidget(AsyncRenderWidget *widget);
	private:
		unordered_map< uint32_t, AsyncRenderWidget * > idToWidget;
		uint32_t freshId;

	//These functions are called to actually kick off async rendering:
	public:
		void queue(AsyncRenderWidget *widget, Camera const &camera, RenderData *data);
		void queue(AsyncRenderWidget *widget, Camera const &camera, Geometry *geometry);
	private:
		std::deque< Job * > computeQueue;
		std::deque< Job * > renderQueue;
		QMutex computeQueueLock;
		QWaitCondition computeQueueHasData;
		QMutex renderQueueLock;
		QWaitCondition renderQueueHasData;

	//Slot used by render threads to return jobs:
	public slots:
		void jobFinished(Job *job);
};

//------------------------------------------
//The ComputeThread takes jobs from the computeQueue, runs their RenderData
// to produce Geometry, and places them onto the renderQueue.

class ComputeThread : public QThread
{
	public:
		ComputeThread(Controller *controller);
		virtual ~ComputeThread();
		virtual void run();

		Controller *controller;
};

//------------------------------------------

//RenderThread defined in its own header.

} //namespace AsyncRenderInternal


#endif //ASYNCRENDERINTERNAL_H
