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
    void pullPlanChanged(PullPlan* p);

protected:
	void dragEnterEvent(QDragEnterEvent* dee);
	void dropEvent(QDropEvent* de);
	void dragMoveEvent(QDragMoveEvent* dme);
	void paintEvent(QPaintEvent *event);
	void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

private:
	void drawSubplan(float x, float y, float width, float height, PullPlan* plan,
		int mandatedShape, int borderLevels, QPainter* painter);
	bool isValidMovePosition(QMouseEvent* event);
    void boundActiveBox();
	PullPlan* plan;
    PullPlan* hoveringPlan;
    int hoveringIndex;
    int activeBoxIndex;
	PullPlan* tempCirclePlan;
	PullPlan* tempSquarePlan;
    QPoint* mouseStartingLoc;
    vector<unsigned int> subplansSelected;
    int activeBox_xmin;
    int activeBox_ymin;
    int activeBox_xmax;
    int activeBox_ymax;

    static const int BOUNDING_BOX_SPACE = 5;

};

#endif // PULLPLANCUSTOMIZEVIEWWIDGET_H
