
#ifndef PICKUPEDITORVIEWWIDGET_H
#define PICKUPEDITORVIEWWIDGET_H

#include <QWidget>

#include "mesh.h"
#include "geometry.h"

class Piece;
class Cane;
class NiceViewWidget;
class UndoRedo;

class PickupEditorViewWidget : public QWidget
{
	Q_OBJECT

	friend class PieceEditorWidget;

	public:
		PickupEditorViewWidget(Piece* piece, UndoRedo* undoRedo, QWidget* parent=0);
		void setPiece(Piece* p);

	public slots:
		void updateEverything();

	protected:
		void mousePressEvent(QMouseEvent* event);
		void dragEnterEvent(QDragEnterEvent* dee);
		void dropEvent(QDropEvent* de);
		void resizeEvent(QResizeEvent* event);

	private:
		Geometry geometry;
		NiceViewWidget* niceViewWidget;
		Piece* piece;
		UndoRedo* undoRedo;
		float ulX, ulY, squareSize;

		float adjustedX(float rawX);
		float adjustedY(float rawX);
		float rawX(float adjustedX);
		float rawY(float adjustedY);
		void subcaneAt(float x, float y, Cane** cane, int* subcaneIndex);
};


#endif

