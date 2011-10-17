
#include "colorbarlibrarywidget.h"

ColorBarLibraryWidget :: ColorBarLibraryWidget(int r, int g, int b, int a, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

	QPixmap image(100, 100);
	image.fill(QColor(r, g, b, a));
	setPixmap(image);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);

	this->pullPlan = new PullPlan();
	this->pullPlan->isBase = true;
	this->pullPlan->setColor(r, g, b, a);
}

PullPlan* ColorBarLibraryWidget :: getPullPlan()
{
	return pullPlan;
}

