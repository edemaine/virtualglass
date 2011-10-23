
#ifndef PIECEEDITORVIEWWIDGET_H
#define PIECEEDITORVIEWWIDGET_H

#include <QtGui>
#include "piece.h"
#include "constants.h"

class PieceEditorViewWidget : public QWidget
{
	Q_OBJECT

	public:
		PieceEditorViewWidget(Piece* piece, QWidget* parent=0);
		void setPiece(Piece* piece);
	
	signals:
		void someDataChanged();

	protected:
		void dragEnterEvent(QDragEnterEvent* dee);
		void dropEvent(QDropEvent* de);
		void paintEvent(QPaintEvent *event);

	private:
		Piece* piece;
		int width;
		int height;	
};


#endif

