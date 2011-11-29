
#include "piecelibrarywidget.h"
#include "qgraphicshighlighteffect.h"

PieceLibraryWidget :: PieceLibraryWidget(QPixmap niceViewPixmap, QPixmap editorPixmap, Piece* piece,
	QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

	setPixmap(niceViewPixmap);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
	this->piece = piece;
	this->piece->setLibraryWidget(this);
	this->editorPixmap = editorPixmap;

	setGraphicsEffect(new QGraphicsHighlightEffect());
}

Piece* PieceLibraryWidget :: getPiece()
{
	return piece;
}

void PieceLibraryWidget :: updatePixmaps(QPixmap niceViewPixmap, QPixmap editorPixmap)
{
		setPixmap(niceViewPixmap);
	this->editorPixmap = editorPixmap;
}

const QPixmap* PieceLibraryWidget :: getEditorPixmap()
{
	return &(this->editorPixmap);
}

