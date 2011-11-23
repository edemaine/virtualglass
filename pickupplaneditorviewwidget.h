
#ifndef PICKUPPLANEDITORVIEWWIDGET_H
#define PICKUPPLANEDITORVIEWWIDGET_H

#include <QtGui>
#include "pickupplan.h"
#include "constants.h"
#include "niceviewwidget.h"
#include "model.h"
#include "piece.h"

class PickupPlanEditorViewWidget : public QWidget
{
	Q_OBJECT

	public:
		PickupPlanEditorViewWidget(Piece* piece, Model* model, QWidget* parent=0);
		void setPiece(Piece* plan);
		QPixmap getPixmap();
	
	signals:
		void someDataChanged();

	protected:
		void dragEnterEvent(QDragEnterEvent* dee);
		void dropEvent(QDropEvent* de);
		void paintEvent(QPaintEvent *event);

	private:
		Model* model;
		NiceViewWidget* niceViewWidget;
		Piece* piece;
};


#endif

