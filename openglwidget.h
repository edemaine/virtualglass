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
		void advanceActiveSubcane();
		void zoomIn();
		void zoomOut();
		void toggleAxes();
		void toggleGrid();
		void setMode(int mode);
		void setCamera(float theta, float fee);
		Point getCameraPoint();
		Vector3f getCameraDirection();
		void saveObjFile(std::string const &filename);
		void setShiftButtonDown(bool state);
		void setBgColor(QColor color);
		void switchView();

	private:
		CaneHistory* history;
		bool shiftButtonDown;
		bool rightMouseDown;
		bool isOrthographic;
		QColor bgColor;
		Model* model;
		Geometry *geometry;
		int resolution;
		bool showAxes;
		bool showGrid;

		float eyeLoc[3];
		float lookAtLoc[3];
		float theta, fee, rho;

		int mouseLocX, mouseLocY;

		GLfloat lightPosition[4];

		void updateCamera();
		void drawAxes();
		void drawGrid();
		void drawTriangle(Triangle* t);
		void updateTriangles();

	signals:
		void zoomInCommandSig();
		void zoomOutCommandSig();
		void modeChangedSig(int mode);

	public slots:
		void caneChangedSlot();
		void zoomInCommandSlot();
		void zoomOutCommandSlot();
		void frontViewCommandSlot();
		void topViewCommandSlot();
		void sideViewCommandSlot();
		void switchProjectionCommandSlot();
		void toggleAxesCommandSlot();
		void toggleGridCommandSlot();

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


