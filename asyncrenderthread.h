#ifndef ASYNCRENDERTHREAD_HPP
#define ASYNCRENDERTHREAD_HPP

#include "glew.h"
#include <QGLWidget>
#include <QThread>

class Camera;
class Geometry;

namespace AsyncRenderInternal {

class Controller;
class Job;

//------------------------------------------
//The RenderThread takes jobs from the renderQueue, shoves their Geometry
// through OpenGL, and delivers them back to the RenderController.

//OpenGL stuff based, to some extent, on http://doc.qt.nokia.com/qq/qq06-glimpsing.html#writingmultithreadedglapplications

class RenderThread : public QThread
{
	Q_OBJECT
public:
	RenderThread(Controller *_controller);
	virtual ~RenderThread();
	virtual void run();

	void setupCamera(Camera const &camera);
	void simpleRender(Geometry const &geom);

	Controller *controller;
	QGLWidget *widget; //kinda silly way of getting a context to work with.

signals:
	void jobFinished(Job *job);
};

} //namespace AsyncRenderInteral

#endif //ASYNCRENDERTHREAD_H
