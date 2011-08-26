
#include "librarycanewidget.h"

LibraryCaneWidget :: LibraryCaneWidget(OpenGLWidget* o, Model* m,
	QWidget* parent=0): QLabel(parent)
{
	model = m;
	cane = model->getCane()->deepCopy();

	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);

	setPixmap(QPixmap::fromImage(o->renderImage()));
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
		emit requestDelete(cane);
	}
}

void LibraryCaneWidget :: deleteRequestAccepted(Cane* c)
{
	if (cane==c)
		this->deleteLater();
}

void LibraryCaneWidget :: enterEvent(QEvent*)
{
	emit mouseOver(this);
}

void LibraryCaneWidget :: newLibraryIndex(int index)
{
	emit newIndex(index);
	cane->changeLibraryIndex(index);
}
