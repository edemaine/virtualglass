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
		OpenGLWidget(QWidget *parent);
		void updateResolution(int new_resolution);
		void setFocusCane(Cane* c);
		void addCane(Cane* c);
		Cane* getCane();
		void advanceActiveSubcane();
		void zeroCanes();
		bool hasCanes();
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
		int mode;
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


