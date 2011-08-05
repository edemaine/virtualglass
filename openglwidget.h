#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include <QtGui>
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
	void updateResolution(int new_resolution);
	void setMode(int mode);
	void setCamera(float theta, float fee);
	Point getCameraPoint();
	Vector3f getCameraDirection();
	void saveObjFile(std::string const &filename);
	void saveRawFile(std::string const &filename);
	void setShiftButtonDown(bool state);
	void setControlButtonDown(bool state);
	void setDeleteButtonDown(bool state);
	void setBgColor(QColor color);
	void setAxes(bool show);
	void setGrid(bool show);
	void setSnaps(bool show);
	void setRefSnaps(bool show);
	void exactInput();
	void setClickable(bool set);
	bool isClickable();

private:
	//CaneHistory* history;
	bool shiftButtonDown;
	bool rightMouseDown;
	bool controlButtonDown;
	bool deleteButtonDown;
	bool isOrthographic;
	QColor bgColor;
	QFormLayout* tableForm;
	QDoubleSpinBox* stretchInput;
	QDoubleSpinBox* twistInput;
	//QDialogButtonBox* buttonsPull;
	QDoubleSpinBox* flattenInput;
	QDoubleSpinBox* rectangleInput;
	//QDialogButtonBox* buttonsFlatten;
	Model* model;
	Geometry *geometry;
	int resolution;
	bool showAxes;
	bool showGrid;
	bool showSnaps;
	bool showRefSnaps;
	bool lockView;
	bool show2D;
	bool clickable;

	float lookAtLoc[3];
	float theta, fee, rho;

	int mouseLocX, mouseLocY;

	int getSubcaneUnderMouse(int mouseX, int mouseY);
	void setGLMatrices();
	void drawAxes();
	void drawGrid();
	void drawSnaps();
	void drawSnapPoints();
	void drawSnapLines();
	void drawSnapCircles();
	void drawTriangle(Triangle* t);
	void updateTriangles();
	Point getClickedPlanePoint(int mouseLocX, int mouseLocY);
	Point getClickedPlanePoint(int mouseLocX, int mouseLocY, float zHeight);
	Point getClickedCanePoint(int activeSubcane,int mouseLocX, int mouseLocY);

signals:
	void modeChangedSig(int mode);
	void operationInfoSig(QString str,int msec);

public slots:
	void zoomIn();
	void zoomOut();
	void caneChanged();
	void setFrontView();
	void setTopView();
	void setSideView();
	void switchProjection();
	void toggleAxes();
	void toggleGrid();
	void toggleSnaps();
	void toggleRefSnaps();
	void lockTable();
	void processPull();
	void processFlatten();
	void toggle2D();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void mousePressEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void wheelEvent(QWheelEvent* e);
};

#endif


