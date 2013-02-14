#ifndef ASYNCPIECELIBRARYWIDGET_H
#define ASYNCPIECELIBRARYWIDGET_H

#include "asyncrenderwidget.h"
#include "piecerenderdata.h"
#include "librarywidget.h"

class Piece;
class QWidget;

class AsyncPieceLibraryWidget : public AsyncRenderWidget
{
	public:
		AsyncPieceLibraryWidget(Piece* _piece, QWidget* parent=0);
		Piece* piece;
		void updatePixmap();
		void updateEyePosition(Vector3f eyePosition);

	private:
		Vector3f eyePosition;
};

#endif
