#ifndef PULLPLANCUSTOMIZEVIEWWIDGET_H
#define PULLPLANCUSTOMIZEVIEWWIDGET_H

#include <QtGui>
#include "constants.h"
#include "pullplan.h"
#include "primitives.h"

class PullPlanCustomizeViewWidget : public QWidget
{
    Q_OBJECT
public:
	PullPlanCustomizeViewWidget(PullPlan* plan, QWidget* parent=0);
	void setPullPlan(PullPlan* plan);
	void revertAndClose();

signals:
	void someDataChanged();

protected:
	void dragEnterEvent(QDragEnterEvent* dee);
	void dropEvent(QDropEvent* de);
	void dragMoveEvent(QDragMoveEvent* dme);
	void paintEvent(QPaintEvent *event);
	void mouseMoveEvent(QMouseEvent* event);

private:
	void drawSubplan(float x, float y, float width, float height, PullPlan* plan,
		int mandatedShape, int borderLevels, QPainter* painter);
	bool isValidMovePosition(QMouseEvent* event);
	PullPlan* plan;
	PullPlan* hoveringPlan;
	int hoveringIndex;
	PullPlan* tempCirclePlan;
	PullPlan* tempSquarePlan;
	QPoint* mouseStartingLoc;
	Point* subpullStartingLoc;

};

#endif // PULLPLANCUSTOMIZEVIEWWIDGET_H
