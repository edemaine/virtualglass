#ifndef PULLPLANLIBRARYWIDGET_H
#define PULLPLANLIBRARYWIDGET_H

#include <QtGui>
#include <QObject>
#include "pullplan.h"
#include "niceviewwidget.h"

class PullPlanLibraryWidget : public QLabel
{
	Q_OBJECT

public:
	PullPlanLibraryWidget(NiceViewWidget* nvw, PullPlan* plan, QWidget* parent=0);
	PullPlan* getPullPlan();

private:
	PullPlan* pullPlan;
};

#endif
