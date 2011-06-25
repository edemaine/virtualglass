
#include "librarycanewidget.h"

LibraryCaneWidget :: LibraryCaneWidget(OpenGLWidget* w, Cane* c, QWidget* parent=0): QLabel(parent)
{
    cane = c;  
    widget = w;

    setBackgroundRole(QPalette::Base);
    setFixedSize(100, 100);
    setScaledContents(true);

    //setPixmap(QPixmap::fromImage(w->grabFrameBuffer()));
    setPixmap(QPixmap::grabWidget(w));

    setAttribute(Qt::WA_LayoutUsesWidgetRect);
}
       

Cane* LibraryCaneWidget :: getCane()
{
    return cane;
}
 

void LibraryCaneWidget :: mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
        return;
    widget->setMode(BUNDLE_MODE);
    widget->addCane(cane);
}

void LibraryCaneWidget :: mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) {
        //QMessageBox::exec();
        this->deleteLater();
    }
}
