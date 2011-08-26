#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <GL/glew.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include <QtGui>
#include <QGLFramebufferObject>
#include <qgl.h>
#include "canehistory.h"
#include "primitives.h"
#include "constants.h"
#include "model.h"
#include "cane.h"
using namespace std;



class OpenGLWidget : public QGLWidget
{
	Q_OBJECT

public:
	Model* getModel();
	OpenGLWidget(QWidget* parent, Model* model);
	virtual ~OpenGLWidget();
	void setMode(int mode);
	void setCamera(float theta, float fee);
	void saveObjFile(std::string const &filename);
	void saveRawFile(std::string const &filename);
	void setShiftButtonDown(bool state);
	void setControlButtonDown(bool state);
	void setDeleteButtonDown(bool state);
	void setBgColor(QColor color);
	void setAxes(bool show);
	void setGrid(bool show);

private:
	bool shiftButtonDown;
	bool rightMouseDown;
	bool deleteButtonDown;
	QColor bgColor;
	Model* model;
	Geometry *geometry;
	bool showAxes;
	bool show2D;

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

	int getSubcaneUnderMouse(int mouseX, int mouseY);
	void setGLMatrices();
	void drawAxes();
	void drawTriangle(Triangle* t);
	void updateTriangles();
	Point getClickedPlanePoint(int mouseLocX, int mouseLocY);
	Point getClickedPlanePoint(int mouseLocX, int mouseLocY, float zHeight);
	Point getClickedCanePoint(int activeSubcane,int mouseLocX, int mouseLocY);

signals:
	void caneChangeRequest(int subcane);
	void modeChangedSig(int mode);
	void operationInfoSig(QString str,int msec);
	void updatePreview();
	void acceptLibraryDelete(Cane* c);
	void updatePeelButton(bool b);

public slots:
	void zoomIn();
	void zoomOut();
	void zoom(float z);
	void caneChanged();
	void setFrontView();
	void setTopView();
	void setSideView();
	void projectionChanged();
	void toggleAxes();
	void toggle2D();
	void togglePeel();
	void processLibraryDelete(Cane* c);

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


