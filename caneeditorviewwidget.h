
#ifndef CANEEDITORVIEWWIDGET_H
#define CANEEDITORVIEWWIDGET_H

#include <QWidget>
#include <set>
#include "primitives.h"
#include "shape.h"

class GlassColor;
class Cane;
class CaneLibraryWidget;
class GlassColorLibraryWidget;
class UndoRedo;

using std::set;

class CaneEditorViewWidget : public QWidget
{
	Q_OBJECT

	public:
		CaneEditorViewWidget(Cane* cane, UndoRedo* undoRedo, QWidget* parent=0);
		void setCane(Cane* cane);
		QRect usedRect();
		static QPixmap renderCane(Cane* cane);
	
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
		void paintShape(Point2D upperLeft, float size, enum GeometricShape s, QPainter* p);
		void setBoundaryPainter(QPainter* painter, bool outermostLevel);
		void drawSubcane(Point2D upperLeft, float width, float height, Cane* cane, 
			bool highlightThis, bool outermostLevel, QPainter* painter);
		void updateHighlightedSubcanesAndCasings(QDragMoveEvent* event);
		bool isOnCasing(int casingIndex, Point2D loc);
		float shapeRadius(enum GeometricShape s, Point2D loc);
		void setMinMaxCasingRadii(float* min, float* max);
		Cane* subcaneAt(Point2D loc);
		int subcaneIndexAt(Point2D loc);
		int casingIndexAt(Point2D loc);
		Point2D mouseToCaneCoords(float x, float y);

		Cane* cane;
		UndoRedo* undoRedo;

		bool isDraggingCasing;
		unsigned int draggedCasingIndex;

		Color highlightColor;
		set<unsigned int> casingsHighlighted;
		set<unsigned int> subcanesHighlighted; 

		Point2D drawUpperLeft;
		float squareSize;

	public slots:
		void updateEverything();
};


#endif

