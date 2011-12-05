
#include "piecelibrarywidget.h"
#include "qgraphicshighlighteffect.h"

PieceLibraryWidget :: PieceLibraryWidget(Piece* piece, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

        QPixmap pixmap(100, 100);
        pixmap.fill(Qt::white);
	setPixmap(pixmap);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
	this->piece = piece;
	this->piece->setLibraryWidget(this);
	this->editorPixmap = pixmap;

	setGraphicsEffect(new QGraphicsHighlightEffect());
	connect(graphicsEffect(),SIGNAL(enabledChanged(bool)),graphicsEffect(),SLOT(setStyleSheet(bool)));
	connect(graphicsEffect(),SIGNAL(styleSheetString(QString)),this,SLOT(setStyleSheet(QString)));
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

