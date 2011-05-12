#include <QtGui>
#include "librarycane.h"
#include "openglwidget.h"
#include "cane.h"

LibraryCane :: LibraryCane(OpenGLWidget* w, Cane* c, QWidget* parent=0): QLabel(parent)
{
        cane = c;  
        widget = w;

        setBackgroundRole(QPalette::Base);
        setFixedSize(100, 100);
        setScaledContents(true);

        setPixmap(QPixmap::fromImage(w->grabFrameBuffer()));
        setAttribute(Qt::WA_LayoutUsesWidgetRect);
}
       

Cane* LibraryCane :: getCane()
{
        return cane;
}
 

void LibraryCane :: mousePressEvent(QMouseEvent* event)
{
        widget->setMode(MOVE_MODE);
        widget->addCane(cane);
}


