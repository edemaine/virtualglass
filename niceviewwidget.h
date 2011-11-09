#ifndef NICEVIEWWIDGET_H
#define NICEVIEWWIDGET_H

#include <GL/glew.h>

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

using namespace std;



class NiceViewWidget : public QGLWidget
{
	Q_OBJECT

public:
	NiceViewWidget(QWidget* parent);
	virtual ~NiceViewWidget();
	void setCameraMode(int m);
	int getCameraMode();
	bool peelEnabled();
	void setGeometry(Geometry* g);

private:
	bool leftMouseDown;
	QColor bgColor;
	Geometry *geometry;

	int cameraMode;
	
	float lookAtLoc[3];
	float theta, fee, rho;

	int mouseLocX, mouseLocY;

	QGLFramebufferObject *selectBuffer;

	bool initializeGLCalled;

	bool peelEnable;
	//various OpenGL objects used when depth peeling:
	const QGLContext *peelInitContext; //context in which all this peel stuff got init'd -- there's something weird going on here with (possibly) copy-constructed versions of the Widget, I'm thinking.
	Vector2ui peelBufferSize;
	GLuint peelBuffer; //framebuffer
	GLuint peelColorTex; //color texture, stores current layer
	GLuint peelDepthTex; //depth texture, stores current depth
	GLuint peelPrevDepthTex; //stores previous depth
	GLhandleARB peelProgram; //program that rejects fragments based on depth
	GLhandleARB nopeelProgram; //program that premultiplies by alpha, but doesn't actually reject

public:
	QImage renderImage();
private:

	void setGLMatrices();
	void drawTriangle(Triangle* t);
	void updateTriangles();

signals:
	void updatePeelButton(bool b);

public slots:
	void zoomIn();
	void zoomOut();
	void zoom(float z);
	void togglePeel();

protected:
	void initializeGL();
	void checkDepthPeel();
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


