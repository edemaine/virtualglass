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
                void updateCane(Cane* c);
                void updateResolution(int new_resolution);
                void updateIlluminatedSubcane(int new_ill_subcane);
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
                Cane* cane;
                int cur_active_subcane;
                Triangle* triangles;
                int num_triangles;
                int resolution;
                
                float eye_loc[3];
                float look_at_loc[3];
                float theta, fee, rho;

                int gNewX, gNewY;

                GLfloat light_position[4];

                void updateCamera();
                void drawTriangle(Triangle* t);


        protected:
                void initializeGL();
                void paintGL();
                void resizeGL(int width, int height);
                void mousePressEvent(QMouseEvent* e);
                void mouseReleaseEvent(QMouseEvent* e);
                void mouseMoveEvent(QMouseEvent* e);
};

#endif


