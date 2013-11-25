
#ifndef PIECELIBRARYWIDGET_H
#define PIECELIBRARYWIDGET_H

#include "asyncrenderwidget.h"
#include "piecerenderdata.h"

class Piece;
class QWidget;

class PieceLibraryWidget : public AsyncRenderWidget
{
	Q_OBJECT

	public:
		PieceLibraryWidget(Piece* _piece, MainWindow* _window);
		Piece* piece;
		const QPixmap* dragPixmap();
		bool isDraggable();
		GlassMime::Type mimeType();
		void updateEyePosition(Vector3f eyePosition);

	public slots:
		void updatePixmap();

	private:
		Vector3f eyePosition;
};

#endif
