
#include "librarycanewidget.h"

LibraryCaneWidget :: LibraryCaneWidget(OpenGLWidget* o, Model* m, 
	QWidget* parent=0): QLabel(parent)
{
	model = m;
	cane = model->getCane()->deepCopy();

	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);

	setPixmap(QPixmap::grabWidget(o));
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
	model->setMode(BUNDLE_MODE);
}

void LibraryCaneWidget :: mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::RightButton) {
		this->deleteLater();
	}
}
