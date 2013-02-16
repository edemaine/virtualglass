
#ifndef PULLPLANEDITORVIEWWIDGET_H
#define PULLPLANEDITORVIEWWIDGET_H

#include <QtGui>
#include <vector>
#include "primitives.h"
#include "shape.h"

class GlassColor;
class PullPlan;
class AsyncPullPlanLibraryWidget;
class AsyncColorBarLibraryWidget;

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
		void paintShape(float x, float y, float size, enum GeometricShape s, QPainter* p);
		float adjustedX(float rawX);
		float adjustedY(float rawX);
		float rawX(float adjustedX);
		float rawY(float adjustedY);
		void setBoundaryPainter(QPainter* painter, bool outermostLevel);
		void drawSubplan(float x, float y, float width, float height, PullPlan* plan, 
			bool highlightThis, bool outermostLevel, QPainter* painter);
		void updateHighlightedSubplansAndCasings(QDropEvent* event);
		void populateHighlightedSubplans(int x, int y, QDropEvent* event);
		void populateHighlightedCasings(int x, int y);
		bool isOnCasing(int casingIndex, float x, float y);
		float getShapeRadius(enum GeometricShape s, float x, float y);
		void setMinMaxCasingRadii(float* min, float* max);
		PullPlan* getSubplanAt(float x, float y);

		PullPlan* plan;

		AsyncColorBarLibraryWidget* draggedLibraryColor;
		GlassColor* draggedColor;
		PullPlan* draggedPlan;

		bool isDraggingCasing;
		unsigned int draggedCasingIndex;

		Color highlightColor;
		vector<unsigned int> casingsHighlighted;
		vector<unsigned int> subplansHighlighted; 

		float ulX, ulY, squareSize;

	public slots:
		void updateEverything();
};


#endif

