
#include "colorbarlibrarywidget.h"
#include "qgraphicshighlighteffect.h"

ColorBarLibraryWidget :: ColorBarLibraryWidget(QPixmap niceViewPixmap, PullPlan* plan, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

	setPixmap(niceViewPixmap);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);

	this->pullPlan = plan;
	this->pullPlan->setColorLibraryWidget(this);
	setGraphicsEffect(new QGraphicsHighlightEffect());
	connect(graphicsEffect(),SIGNAL(enabledChanged(bool)),this,SLOT(setStyleSheet(bool)));

	connect(this,SIGNAL(styleSheetString(QString)),this,SLOT(setStyleSheet(QString)));
}

void ColorBarLibraryWidget :: setStyleSheet(bool enableBorder)
{
	if (enableBorder)
		emit styleSheetString("border: 1px solid "+((QGraphicsHighlightEffect*)graphicsEffect())->color().name()+";");
	else
		emit styleSheetString("border: 0px solid "+((QGraphicsHighlightEffect*)graphicsEffect())->color().name()+";");
}

void ColorBarLibraryWidget :: updatePixmap(QPixmap niceViewPixmap)
{
	setPixmap(niceViewPixmap);
}

PullPlan* ColorBarLibraryWidget :: getPullPlan()
{
	return pullPlan;
}

void ColorBarLibraryWidget :: addPullPlan(PullPlan *plan)
{
	pullPlans.push_back(plan);
}
