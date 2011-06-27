
#include "librarycanewidget.h"

LibraryCaneWidget :: LibraryCaneWidget(OpenGLWidget* w, Model* m, Cane* c, QWidget* parent=0): QLabel(parent)
{
	cane = c;
	model = m;

	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);

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
	model->addCane(cane);
}

void LibraryCaneWidget :: mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::RightButton) {
		this->deleteLater();
	}
}
