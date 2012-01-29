#ifndef PULLPLANCUSTOMIZEWIDGET_H
#define PULLPLANCUSTOMIZEWIDGET_H

#include <QtGui>
#include "constants.h"
#include "pullplan.h"
#include "pullplancustomizeviewwidget.h"

class PullPlanCustomizeWidget : public QWidget
{
	Q_OBJECT

	public:
		PullPlanCustomizeWidget(PullPlan* plan, QWidget* parent=0);
//		PullPlan* getPlan();
		void setPullPlan(PullPlan* p);
		void mousePressEvent(QMouseEvent*);
		void openWindow(PullPlan* plan);

	signals:
		void someDataChanged();
		void newPullPlan(PullPlan* p);

	public slots:
//		void updateEverything();
		void cancelButtonPressed();

	private slots:
//		void viewWidgetDataChanged();

	private:
		PullPlanCustomizeViewWidget* viewWidget;
//		NiceViewWidget* niceViewWidget;
		QPushButton* confirmChangesButton;
		QPushButton* cancelButton;

		void setupLayout();
		void setupConnections();

};

#endif // PULLPLANCUSTOMIZEWIDGET_H
