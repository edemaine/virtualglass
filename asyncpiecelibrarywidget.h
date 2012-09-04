#ifndef ASYNCPIECELIBRARYWIDGET_H
#define ASYNCPIECELIBRARYWIDGET_H

#include <QtGui>
#include "asyncrenderwidget.h"

class Piece;

class AsyncPieceLibraryWidget : public AsyncRenderWidget
{
	public:
		AsyncPieceLibraryWidget(Piece* _piece, QWidget* parent=0);
		Piece* getPiece();
		void updatePixmap();
		void updateEyePosition(Vector3f eyePosition);

	private:
		Piece* piece;
		Vector3f eyePosition;
};

#endif
