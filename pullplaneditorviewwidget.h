
#ifndef PULLPLANEDITORVIEWWIDGET_H
#define PULLPLANEDITORVIEWWIDGET_H

#include <QtGui>
#include "pullplan.h"

class PullPlanEditorViewWidget : public QWidget
{
	Q_OBJECT

	public:
		PullPlanEditorViewWidget(PullPlan* plan, QWidget* parent=0);
		void setPullPlan(PullPlan* plan);
	
	signals:
		void someDataChanged();

	protected:
		void dragEnterEvent(QDragEnterEvent* dee);
		void dropEvent(QDropEvent* de);
		void paintEvent(QPaintEvent *event);

	private:
		void drawSubplan(int x, int y, int width, int height, PullPlan* plan, int mandatedShape, QPainter* painter);
		PullPlan* plan;
};


#endif

