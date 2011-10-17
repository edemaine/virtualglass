#ifndef COLORBARLIBRARYWIDGET_H
#define COLORBARLIBRARYWIDGET_H

#include <QtGui>
#include <QObject>
#include "pullplan.h"

class ColorBarLibraryWidget : public QLabel
{
	Q_OBJECT

	public:
		ColorBarLibraryWidget(int r, int g, int b, int a, QWidget* parent=0);
		PullPlan* getPullPlan();	

	private:
		PullPlan* pullPlan;
};

#endif
