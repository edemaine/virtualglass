
#ifndef COLOREDITORVIEWWIDGET_H
#define COLOREDITORVIEWWIDGET_H

#include <QtGui>
#include "pullplan.h"

class ColorEditorViewWidget : public QWidget
{
	Q_OBJECT

	public:
		ColorEditorViewWidget(PullPlan* plan, QWidget* parent=0);
		void setPullPlan(PullPlan* plan);
	
	signals:
		void someDataChanged();

	private:
		PullPlan* plan;
};


#endif

