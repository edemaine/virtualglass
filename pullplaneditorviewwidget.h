
#ifndef PULLPLANEDITORVIEWWIDGET_H
#define PULLPLANEDITORVIEWWIDGET_H

#include <QtGui>
#include "constants.h"
#include "pullplan.h"

class PullPlanEditorViewWidget : public QWidget
{
	Q_OBJECT

	public:
		PullPlanEditorViewWidget(PullPlan* plan, QWidget* parent=0);
		void setPullPlan(PullPlan* plan);
		void setFillRule(int r);
	
	signals:
		void someDataChanged();

	protected:
		void dragEnterEvent(QDragEnterEvent* dee);
		void dropEvent(QDropEvent* de);
		void paintEvent(QPaintEvent *event);

	private:
		void drawSubplan(float x, float y, float width, float height, PullPlan* plan, 
			int mandatedShape, int borderLevels, QPainter* painter);
		PullPlan* plan;
		int fill_rule;
};


#endif

