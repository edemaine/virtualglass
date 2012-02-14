#ifndef ASYNCPIECELIBRARYWIDGET_H
#define ASYNCPIECELIBRARYWIDGET_H

#include <QtGui>
#include <QObject>
#include "asyncrenderwidget.h"

class Piece;

class AsyncPieceLibraryWidget : public AsyncRenderWidget
{
	Q_OBJECT

	public:
		AsyncPieceLibraryWidget(Piece* _piece, QWidget* parent=0);
		Piece* getPiece();
		void updatePixmap(Vector3f eye_loc);

	private:
		Piece* piece;
};

#endif
