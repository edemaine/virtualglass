#ifndef COLORBARLIBRARYWIDGET_H
#define COLORBARLIBRARYWIDGET_H

#include <QtGui>
#include <QObject>
#include "pullplan.h"

class ColorBarLibraryWidget : public QLabel
{
	Q_OBJECT

	public:
		ColorBarLibraryWidget(PullPlan* plan, QWidget* parent=0);
		PullPlan* getPullPlan();	

	private:
		PullPlan* pullPlan;
};

#endif
