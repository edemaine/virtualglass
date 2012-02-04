#ifndef ASYNCRENDERWIDGET_H
#define ASYNCRENDERWIDGET_H

#include "Vector.hpp"

#include <QtGui>

class Geometry;

class RenderData
{
public:
	//may be called in a separate thread:
	virtual Geometry *getGeometry() = 0;
};

class Camera
{
public:
	//camera is at eye, looking at 'lookAt' with up vector close to 'up'
	//if isPerspective camera has 45 degree vertical fov
	//else camera vertically spans length(eye - lookAt) / 2.2 of the scene
	Vector3f eye;
	Vector3f lookAt;
	Vector3f up;
	bool isPerspective;
	Vector2ui size; //size of desired image, in pixels
};

class AsyncRenderWidget : public QLabel
{
	Q_OBJECT
public:
	AsyncRenderWidget(QWidget *parent=NULL);
	virtual ~AsyncRenderWidget();

	//The AsyncRenderWidget now owns the 'RenderData' instance and will delete it:
	//It is an error to pass NULL as data.
	void setScene(Camera const &camera, RenderData *data);

	//Adjust just the camera, keep previous geometry:
	void setCamera(Camera const &);

	//deletes current geometry without supplying new geometry:
	void clearGeometry();

	uint32_t id; //used to track this widget.

	//callback invoked by render thread; widget is responsible for deleting everything:
	void renderFinished(Camera const &camera, RenderData *data, Geometry *geometry, QImage *image);

};

#endif //ASYNCRENDERWIDGET_H
