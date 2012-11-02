#ifndef PULLPLANCUSTOMIZEVIEWWIDGET_H
#define PULLPLANCUSTOMIZEVIEWWIDGET_H

#include <QtGui>
#include <QtDebug>
#include <cfloat>
#include <vector>
#include "primitives.h"
#include "shape.h"

class GlassColor;
class PullPlan;

using std::vector;

class PullPlanCustomizeViewWidget : public QWidget
{
	Q_OBJECT

public:
	PullPlanCustomizeViewWidget(PullPlan* plan, QWidget* parent=0);
	void setPullPlan(PullPlan* plan);
	void revertAndClose();
	void addCirclePressed();
	void addSquarePressed();
	void copySelectionPressed();
	void deleteSelectionPressed();

signals:
	void someDataChanged();
	void pullPlanChanged(PullPlan* p);

protected:
	void dropEvent(QDropEvent* de);
	void dragMoveEvent(QDragMoveEvent* dme);
	void paintEvent(QPaintEvent *event);
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void resizeEvent(QResizeEvent* event);
	void keyPressEvent(QKeyEvent* event);

private:
	enum GUIMode
	{
		MOVE_MODE,
		SCALE_MODE
	};

	float adjustedX(float rawX);
	float adjustedY(float rawX);
	float rawX(float adjustedX);
	float rawY(float adjustedY);
	void drawSubplan(float x, float y, float drawWidth, float drawHeight,
		PullPlan* plan, bool outermostLevel, QPainter* painter);
	void paintShape(float x, float y, float size, enum GeometricShape s, QPainter* painter);
	void setBoundaryPainter(QPainter* painter, bool outermostLevel, bool greyedOut);
	void drawActionControls(QPainter* painter);
	bool isValidMovePosition(QMouseEvent* event);
	void boundActiveBox();
	void updateIndexes(QPoint pos);

	PullPlan* plan;
	PullPlan* hoveringPlan;
	GUIMode mode;
	int hoveringIndex;
	int activeBoxIndex;
	Point mouseStartingLoc;
	QPoint* clickedLoc;
	bool clickMoved;
	vector<unsigned int> subplansSelected;
	int activeBox_xmin;
	int activeBox_ymin;
	int activeBox_xmax;
	int activeBox_ymax;
	int activeControlPoint;
	float ulX, ulY, squareSize;

	int BOUNDING_BOX_SPACE;

};

#endif // PULLPLANCUSTOMIZEVIEWWIDGET_H
