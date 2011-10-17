
#include "pullplanlibrarywidget.h"

PullPlanLibraryWidget :: PullPlanLibraryWidget(QPixmap niceViewPixmap, QPixmap editorPixmap, PullPlan* plan, 
	QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

	setPixmap(niceViewPixmap);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);

	this->pullPlan = plan->deepCopy();
	this->editorPixmap = editorPixmap;
}

PullPlan* PullPlanLibraryWidget :: getPullPlan()
{
	return pullPlan;
}

QPixmap PullPlanLibraryWidget :: getEditorPixmap()
{
	return this->editorPixmap;
}

