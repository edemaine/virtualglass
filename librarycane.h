#ifndef LIBRARYCANE_H
#define LIBRARYCANE_H

#include <QtGui>
#include "openglwidget.h"
#include "cane.h"

class LibraryCane : public QLabel
{
        public:
                LibraryCane(OpenGLWidget* w, Cane* c, QWidget* parent);
                Cane* getCane();
                void mousePressEvent(QMouseEvent* event);

        protected:
                Cane* cane;
                OpenGLWidget* widget;
};

#endif

