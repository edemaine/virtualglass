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
	PullPlanLibraryWidget(NiceViewWidget* nvw, QWidget* parent=0);
	//void mousePressEvent(QMouseEvent* event);
	//void mouseReleaseEvent(QMouseEvent* event);
	PullPlan* getPullPlan();

private:
	PullPlan* pullPlan;
};

#endif
