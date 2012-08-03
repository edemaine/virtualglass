
#ifndef PULLPLANEDITORVIEWWIDGET_H
#define PULLPLANEDITORVIEWWIDGET_H

#include <QtGui>
#include <vector>
#include "constants.h"
#include "pullplan.h"
#include "asyncpullplanlibrarywidget.h"
#include "asynccolorbarlibrarywidget.h"
#include "glassmime.h"

using std::vector;

class PullPlanEditorViewWidget : public QWidget
{
	Q_OBJECT

	public:
		PullPlanEditorViewWidget(PullPlan* plan, QWidget* parent=0);
		void setPullPlan(PullPlan* plan);
		QRect usedRect();
		static QPixmap renderPullPlan(PullPlan* plan);
	
	signals:
		void someDataChanged();

	protected:
		void mousePressEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
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
		void updateHighlightedSubplansAndCasings(QDropEvent* event);
		void populateHighlightedSubplans(int x, int y, QDropEvent* event);
		void populateHighlightedCasings(int x, int y);
		bool isOnCasing(int casingIndex, float x, float y);
		float getShapeRadius(int shape, float x, float y);
		void setMinMaxCasingRadii(float* min, float* max);
		PullPlan* getSubplanAt(float x, float y);

		PullPlan* plan;
		Color draggingColor;

		bool isDraggingPlan;
		int draggedPlanType;
		PullPlan* draggedPlan;

		bool isDraggingCasing;
		unsigned int draggedCasingIndex;

		bool casingHighlighted;
		unsigned int casingHighlightIndex;
		vector<unsigned int> subplansHighlighted; 
		float ulX, ulY, squareSize;
};


#endif

