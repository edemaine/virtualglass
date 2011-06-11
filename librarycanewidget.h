#ifndef LIBRARYCANEWIDGET_H
#define LIBRARYCANEWIDGET_H

#include <QtGui>
#include "openglwidget.h"
#include "cane.h"

class LibraryCaneWidget : public QLabel
{
        public:
                LibraryCaneWidget(OpenGLWidget* w, Cane* c, QWidget* parent);
                Cane* getCane();
                void mousePressEvent(QMouseEvent* event);
                void mouseReleaseEvent(QMouseEvent* event);

        protected:
                Cane* cane;
                OpenGLWidget* widget;
};

#endif
