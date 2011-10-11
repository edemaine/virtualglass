
#include "pullplanlibrarywidget.h"

PullPlanLibraryWidget :: PullPlanLibraryWidget(NiceViewWidget* niceViewWidget, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

	setPixmap(QPixmap::fromImage(niceViewWidget->renderImage()).scaled(100, 100));
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
}

PullPlan* PullPlanLibraryWidget :: getPullPlan()
{
	return pullPlan;
}

