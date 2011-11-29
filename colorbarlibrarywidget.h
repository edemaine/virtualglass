#ifndef COLORBARLIBRARYWIDGET_H
#define COLORBARLIBRARYWIDGET_H

#include <QtGui>
#include <QObject>
#include "pullplan.h"

class ColorBarLibraryWidget : public QLabel
{
	Q_OBJECT

	public:
		ColorBarLibraryWidget(QPixmap niceViewPixmap, PullPlan* plan, QWidget* parent=0);
		PullPlan* getPullPlan();
		void updatePixmap(QPixmap niceViewPixmap);
		void addPullPlan(PullPlan* plan);
		vector<PullPlan*> pullPlans;

	private:
		QButtonGroup* colorBarTemplateShapeButtonGroup;
		PullPlan* pullPlan;
};

#endif
