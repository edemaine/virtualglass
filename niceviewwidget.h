#ifndef NICEVIEWWIDGET_H
#define NICEVIEWWIDGET_H

#include "glew.h"

#ifdef _WIN32
#  include <windows.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include <QtGui>
#include <QGLFramebufferObject>
#include <qgl.h>
#include "primitives.h"
#include "constants.h"
#include "geometry.h"
#include "peelrenderer.h"

using namespace std;

class NiceViewWidget : public QGLWidget
{
	Q_OBJECT

public:
        enum CameraMode
        {
                PULLPLAN_CAMERA_MODE,
		PICKUPPLAN_CAMERA_MODE,
               	PIECE_CAMERA_MODE 
        };

	NiceViewWidget(enum CameraMode cameraMode, QWidget* parent=0);
	virtual ~NiceViewWidget();
	bool peelEnabled();
	void setGeometry(Geometry* g);

	Vector3f eyePosition();


private:
	bool leftMouseDown;
	QColor bgColor;
	Geometry *geometry;

	enum CameraMode cameraMode;

	float lookAtLoc[3];
	float theta, phi, rho;

	int mouseLocX, mouseLocY;

	QGLFramebufferObject *selectBuffer;

	bool initializeGLCalled;

	PeelRenderer *peelRenderer;

public:
	QImage renderImage();
private:

	void setGLMatrices();
	void drawTriangle(Triangle* t);
	void updateTriangles();

public slots:
	void zoomIn();
	void zoomOut();
	void zoom(float z);

protected:
	void initializeGL();
	void paintWithDepthPeeling();
	void paintWithoutDepthPeeling();
	void paintGL();
	void resizeGL(int width, int height);
	void mousePressEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void wheelEvent(QWheelEvent* e);
};

#endif


