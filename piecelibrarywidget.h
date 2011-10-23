#ifndef PIECELIBRARYWIDGET_H
#define PIECELIBRARYWIDGET_H

#include <QtGui>
#include <QObject>
#include "piece.h"

class PieceLibraryWidget : public QLabel
{
	Q_OBJECT

	public:
		PieceLibraryWidget(QPixmap niceViewPixmap, QPixmap editorPixmap, Piece* piece, QWidget* parent=0);
		Piece* getPiece();
		const QPixmap* getEditorPixmap();
		void updatePixmaps(QPixmap niceViewPixmap, QPixmap editorPixmap);

	private:
		Piece* piece;
		QPixmap editorPixmap;
};

#endif
