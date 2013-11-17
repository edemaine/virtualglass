#ifndef CANECUSTOMIZEVIEWWIDGET_H
#define CANECUSTOMIZEVIEWWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QDragMoveEvent>
#include <cfloat>
#include <vector>

#include "primitives.h"
#include "shape.h"

class QPainter;
class GlassColor;
class PullPlan;

using std::vector;

class CaneCustomizeViewWidget : public QWidget
{
	Q_OBJECT

	public:
		CaneCustomizeViewWidget(PullPlan* plan, QWidget* parent=0);
		void setPullPlan(PullPlan* plan);
		void addCircleClicked();
		void addSquareClicked();
		void copySelectionClicked();
		void deleteSelectionClicked();
	
	signals:
		void someDataChanged();
		void pullPlanChanged(PullPlan* p);

	public slots:
		void updateEverything();

	protected:
		void paintEvent(QPaintEvent *event);
		void mousePressEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
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
		void drawSubplan(Point2D upperLeft, float drawWidth, float drawHeight,
			PullPlan* plan, bool outermostLevel, QPainter* painter);
		void paintShape(Point2D upperLeft, float size, enum GeometricShape s, QPainter* painter);
		void setBoundaryPainter(QPainter* painter, bool outermostLevel, bool greyedOut);
		void drawActionControls(QPainter* painter);
		bool isValidMovePosition(QMouseEvent* event);
		void boundActiveBox();
		void updateIndexes(QPoint pos);
		void deleteSelection();
		void copySelection();
	
		GUIMode mode;
		PullPlan* plan;
		PullPlan* hoveringPlan;
		int hoveringIndex;
		int activeBoxIndex;
		Point2D mouseStartingLoc;
		Point2D clickedLoc;
		bool clickMoved;
		vector<unsigned int> subplansSelected;
		Point2D activeBoxLL;
		Point2D activeBoxUR;
		int activeControlPoint;
		Point2D drawUpperLeft;
		float squareSize;

		int boundingBoxSpace;

	public slots:
		void setCustomTemplate();
};

#endif // PULLPLANCUSTOMIZEVIEWWIDGET_H
