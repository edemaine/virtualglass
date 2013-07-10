
#ifndef PICKUPPLANEDITORVIEWWIDGET_H
#define PICKUPPLANEDITORVIEWWIDGET_H

#include <QWidget>

#include "mesh.h"
#include "geometry.h"

class Piece;
class PullPlan;
class NiceViewWidget;

class PickupPlanEditorViewWidget : public QWidget
{
	Q_OBJECT

	friend class PieceEditorWidget;

	public:
		PickupPlanEditorViewWidget(Piece* piece, QWidget* parent=0);
		void setPiece(Piece* p);

	signals:
		void someDataChanged();

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
		float ulX, ulY, squareSize;

		void setupConnections();
		float adjustedX(float rawX);
		float adjustedY(float rawX);
		float rawX(float adjustedX);
		float rawY(float adjustedY);
		void getSubplanAt(float x, float y, PullPlan** plan, int* subplanIndex);
};


#endif

