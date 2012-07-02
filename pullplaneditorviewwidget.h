
#ifndef PULLPLANEDITORVIEWWIDGET_H
#define PULLPLANEDITORVIEWWIDGET_H

#include <QtGui>
#include <vector>
#include "constants.h"
#include "pullplan.h"

using std::vector;

class PullPlanEditorViewWidget : public QWidget
{
	Q_OBJECT

	public:
		PullPlanEditorViewWidget(PullPlan* plan, QWidget* parent=0);
		void setPullPlan(PullPlan* plan);
		void setFillRule(int r);
		int getFillRule();
		void mousePressEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		QRect usedRect();
		static QPixmap renderPullPlan(PullPlan* plan);
	
	signals:
		void someDataChanged();

	protected:
		void dropEvent(QDropEvent* de);
		void dragLeaveEvent(QDragLeaveEvent* dle);
		void dragEnterEvent(QDragEnterEvent* dee);
		void dragMoveEvent(QDragMoveEvent* dme);
		void paintEvent(QPaintEvent *event);
		void resizeEvent(QResizeEvent* event);

	private:
		void paintShape(float x, float y, float size, int shape, QPainter* painter);
		float adjustedX(float rawX);
		float adjustedY(float rawX);
		float rawX(float adjustedX);
		float rawY(float adjustedY);
		void setBoundaryPainter(QPainter* painter, bool outermostLevel);
		void drawSubplan(float x, float y, float width, float height, PullPlan* plan, 
			bool highlightThis, int mandatedShape, bool outermostLevel, QPainter* painter);
		PullPlan* plan;
		int fill_rule;
		Color draggingColor;
		bool isDraggingCasing;
		unsigned int draggedCasingIndex;
		void populateHighlightedSubplans(int x, int y, PullPlan* plan, int type);
		void populateHighlightedCasings(int x, int y, int type);
		bool casingHighlighted;
		unsigned int casingHighlightIndex;
		vector<unsigned int> subplansHighlighted; 
		float ulX, ulY, squareSize;
};


#endif

