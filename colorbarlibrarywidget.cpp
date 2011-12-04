
#include "colorbarlibrarywidget.h"
#include "qgraphicshighlighteffect.h"

ColorBarLibraryWidget :: ColorBarLibraryWidget(PullPlan* plan, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

        QPixmap pixmap(100, 100);
        pixmap.fill(Qt::white);
	setPixmap(pixmap);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);

	this->pullPlan = plan;
	this->pullPlan->setColorLibraryWidget(this);
	setGraphicsEffect(new QGraphicsHighlightEffect());
	connect(graphicsEffect(),SIGNAL(enabledChanged(bool)),graphicsEffect(),SLOT(setStyleSheet(bool)));
	connect(graphicsEffect(),SIGNAL(styleSheetString(QString)),this,SLOT(setStyleSheet(QString)));
}

/*void ColorBarLibraryWidget :: setStyleSheet(bool enableBorder)
{
	if (enableBorder)
		emit styleSheetString("border: 1px solid "+((QGraphicsHighlightEffect*)graphicsEffect())->color().name()+";");
	else
		emit styleSheetString("border: 0px solid "+((QGraphicsHighlightEffect*)graphicsEffect())->color().name()+";");
}*/

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
