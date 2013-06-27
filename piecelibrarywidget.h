#ifndef PIECELIBRARYWIDGET_H
#define PIECELIBRARYWIDGET_H

#include "asyncrenderwidget.h"
#include "piecerenderdata.h"

class Piece;
class QWidget;

class PieceLibraryWidget : public AsyncRenderWidget
{
	public:
		PieceLibraryWidget(Piece* _piece, MainWindow* _window);
		Piece* piece;
		void updatePixmap();
		void updateEyePosition(Vector3f eyePosition);

	private:
		Vector3f eyePosition;
};

#endif
