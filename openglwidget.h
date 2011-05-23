#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <QtGui>
#include <qgl.h>
#include "primitives.h"
#include "constants.h"
#include "mesh.h"
#include "cane.h"

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
                void zoomIn();
                void zoomOut();
                void setMode(int mode);

        private:
                Mesh* mesh;
                int mode;
                Triangle* triangles;
                int num_triangles;
                int resolution;
                
                float eyeLoc[3];
                float lookAtLoc[3];
                float theta, fee, rho;

                int mouseLocX, mouseLocY;

                GLfloat lightPosition[4];

                void updateCamera();
                void drawTriangle(Triangle* t);
                void updateTriangles();

        protected:
                void initializeGL();
                void paintGL();
                void resizeGL(int width, int height);
                void mousePressEvent(QMouseEvent* e);
                void mouseReleaseEvent(QMouseEvent* e);
                void mouseMoveEvent(QMouseEvent* e);
};

#endif


