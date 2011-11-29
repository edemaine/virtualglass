
#include "pullplanlibrarywidget.h"
#include "qgraphicshighlighteffect.h"

PullPlanLibraryWidget :: PullPlanLibraryWidget(QPixmap niceViewPixmap, QPixmap editorPixmap, PullPlan* plan,
	QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

	setPixmap(niceViewPixmap);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
	this->pullPlan = plan;
	this->pullPlan->setLibraryWidget(this);
	this->editorPixmap = editorPixmap;

	setGraphicsEffect(new QGraphicsHighlightEffect());
}

PullPlan* PullPlanLibraryWidget :: getPullPlan()
{
	return pullPlan;
}

void PullPlanLibraryWidget :: updatePixmaps(QPixmap niceViewPixmap, QPixmap editorPixmap)
{
	setPixmap(niceViewPixmap);
	this->editorPixmap = editorPixmap;
	getPullPlan()->updatePixmap(editorPixmap);
}

const QPixmap* PullPlanLibraryWidget :: getEditorPixmap()
{
	return &(this->editorPixmap);
}

