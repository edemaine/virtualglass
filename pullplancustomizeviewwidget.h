#ifndef PULLPLANCUSTOMIZEVIEWWIDGET_H
#define PULLPLANCUSTOMIZEVIEWWIDGET_H

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

class PullPlanCustomizeViewWidget : public QWidget
{
	Q_OBJECT

	public:
		PullPlanCustomizeViewWidget(PullPlan* plan, QWidget* parent=0);
		void setPullPlan(PullPlan* plan);
		void revertAndClose();
		void addCircleClicked();
		void addSquareClicked();
		void copySelectionClicked();
		void deleteSelectionClicked();
		void updateEverything();

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
		void drawSubplan(Point2D upperLeft, float drawWidth, float drawHeight,
			PullPlan* plan, bool outermostLevel, QPainter* painter);
		void paintShape(Point2D upperLeft, float size, enum GeometricShape s, QPainter* painter);
		void setBoundaryPainter(QPainter* painter, bool outermostLevel, bool greyedOut);
		void drawActionControls(QPainter* painter);
		bool isValidMovePosition(QMouseEvent* event);
		void boundActiveBox();
		void updateIndexes(QPoint pos);

		GUIMode mode;
		PullPlan* plan;
		PullPlan* hoveringPlan;
		int hoveringIndex;
		int activeBoxIndex;
		Point2D mouseStartingLoc;
		QPoint* clickedLoc;
		bool clickMoved;
		vector<unsigned int> subplansSelected;
		int activeBox_xmin;
		int activeBox_ymin;
		int activeBox_xmax;
		int activeBox_ymax;
		int activeControlPoint;
		Point2D drawUpperLeft;
		float squareSize;

		int BOUNDING_BOX_SPACE;

	public slots:
		void setCustomTemplate();
};

#endif // PULLPLANCUSTOMIZEVIEWWIDGET_H
