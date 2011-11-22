
#include "colorbarlibrarywidget.h"

ColorBarLibraryWidget :: ColorBarLibraryWidget(QPixmap niceViewPixmap, PullPlan* plan, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

	setPixmap(niceViewPixmap);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);

	this->pullPlan = plan;
	this->pullPlan->setLibraryWidget(this);
	setGraphicsEffect(new QGraphicsColorizeEffect());
	//graphicsEffect()->setEnabled(false);
}

void ColorBarLibraryWidget :: updatePixmap(QPixmap niceViewPixmap)
{
	setPixmap(niceViewPixmap);
}

PullPlan* ColorBarLibraryWidget :: getPullPlan()
{
	return pullPlan;
}


