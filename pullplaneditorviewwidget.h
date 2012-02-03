
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
	
	signals:
		void someDataChanged();

	protected:
		void dropEvent(QDropEvent* de);
		void dragLeaveEvent(QDragLeaveEvent* dle);
		void dragEnterEvent(QDragEnterEvent* dee);
		void dragMoveEvent(QDragMoveEvent* dme);
		void paintEvent(QPaintEvent *event);

	private:
		void setBoundaryPainter(QPainter* painter, int drawWidth, int drawHeight, 
			int borderLevels);
		void drawSubplan(float x, float y, float width, float height, PullPlan* plan, 
			bool highlightThis, int mandatedShape, int borderLevels, QPainter* painter);
		PullPlan* plan;
		int fill_rule;
		bool isDraggingCasing;
		unsigned int draggedCasingIndex;
		void populateHighlightedSubplans(int x, int y, PullPlan* plan, int type);
		void populateIsCasingHighlighted(int x, int y, int type);
		bool casingHighlighted;
		unsigned int casingHighlightIndex;
		vector<unsigned int> subplansHighlighted; 
};


#endif

