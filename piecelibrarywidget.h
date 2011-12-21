#ifndef PIECELIBRARYWIDGET_H
#define PIECELIBRARYWIDGET_H

class Piece;

#include <QtGui>
#include <QObject>
#include "piece.h"

class PieceLibraryWidget : public QLabel
{
	Q_OBJECT

	public:
		PieceLibraryWidget(Piece* piece, QWidget* parent=0);
		Piece* getPiece();
		const QPixmap* getEditorPixmap();
		void updatePixmap(QPixmap pixmap);

	private:
		Piece* piece;
};

#endif
