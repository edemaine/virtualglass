#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <qgl.h>
#include <qevent.h>
#include "primitives.h"
#include "constants.h"
#include "convert.h"
#include "cane.h"

#define LOOK_MODE 1
#define TWIST_MODE 2
#define STRETCH_MODE 3
#define MOVE_MODE 4

class OpenGLWidget : public QGLWidget
{
        Q_OBJECT
  
        public:
                OpenGLWidget(QWidget *parent);
                void twistCane(float radians);
                void updateCane();
                void setFocusCane(Cane* c);
                void addCane(Cane* c);
                Cane* getCane();
                void advanceActiveSubcane();
                void zeroCanes();
                void zoomIn();
                void zoomOut();
                void setMode(int mode);

        private:
                Triangle* mesh; 
                int num_mesh_elements; 
                int mode;
                Cane* cane;
                int cur_active_subcane;
                int mousePressed;
                
                float eye_loc[3];
                float look_at_loc[3];
                float theta, fee, rho;

                int gLeftDown;
                int gShiftLeftDown;
                int gAltLeftDown;
                int gNewX, gNewY;

                GLfloat light_position[4];

                float abs(float v);
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


