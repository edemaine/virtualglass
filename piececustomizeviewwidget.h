
#ifndef PIECECUSTOMIZEVIEWWIDGET_H
#define PIECECUSTOMIZEVIEWWIDGET_H

#include <QWidget>
#include "primitives.h"

class Piece;

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
		float rawScale(float adjustedScale);
		float adjustedScale(float rawScale);
		Point controlPointRawLocation(unsigned int index);
		void drawPiece();

		Piece* piece;

		bool isDraggingControlPoint;
		unsigned int draggedControlPointIndex;
		float ulX, ulY, squareSize;

	public slots:
		void updateEverything();
};


#endif

