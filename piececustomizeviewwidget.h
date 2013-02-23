
#ifndef PIECECUSTOMIZEVIEWWIDGET_H
#define PIECECUSTOMIZEVIEWWIDGET_H

#include <QWidget>
#include <vector>
#include "primitives.h"
#include "shape.h"

class Piece;

using std::vector;

class PieceCustomizeViewWidget : public QWidget
{
	Q_OBJECT

	public:
		PieceCustomizeViewWidget(Piece* piece, QWidget* parent=0);
		void setPiece(Piece* piece);
		QRect usedRect();
	
	signals:
		void someDataChanged();

	protected:
		void mousePressEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void paintEvent(QPaintEvent *event);
		void resizeEvent(QResizeEvent* event);

	private:
		float adjustedX(float rawX);
		float adjustedY(float rawX);
		float rawX(float adjustedX);
		float rawY(float adjustedY);
		void drawPiece();

		Piece* piece;

		bool isDraggingPoint;
		unsigned int draggedPointIndex;

		float ulX, ulY, squareSize;

	public slots:
		void updateEverything();
};


#endif

