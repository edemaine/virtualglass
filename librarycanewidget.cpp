
#include "librarycanewidget.h"

LibraryCaneWidget :: LibraryCaneWidget(OpenGLWidget* w, Cane* c, QWidget* parent=0): QLabel(parent)
{
        cane = c;  
        widget = w;

        setBackgroundRole(QPalette::Base);
        setFixedSize(100, 100);
        setScaledContents(true);

        setPixmap(QPixmap::fromImage(w->grabFrameBuffer()));
        setAttribute(Qt::WA_LayoutUsesWidgetRect);
}
       

Cane* LibraryCaneWidget :: getCane()
{
        return cane;
}
 

void LibraryCaneWidget :: mousePressEvent(QMouseEvent* event)
{
        widget->setMode(BUNDLE_MODE);
        widget->addCane(cane);
}


