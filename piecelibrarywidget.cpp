

#include "piecelibrarywidget.h"

PieceLibraryWidget :: PieceLibraryWidget(Piece* _piece, MainWindow* _window)
	: AsyncRenderWidget(_window), piece(_piece)
{
	setFixedSize(100, 100);
	eyePosition.x = -16.0;
	eyePosition.y = 0.0;
	eyePosition.z = 0.0;
	updatePixmap();
}

void PieceLibraryWidget :: updateEyePosition(Vector3f _eyePosition)
{
	eyePosition = _eyePosition;
}

void PieceLibraryWidget :: updatePixmap()
{
	//queue up an async update:
	Camera camera;
	camera.eye = eyePosition;
	camera.lookAt = make_vector(0.0f, 0.0f, 0.0f);
	camera.up = make_vector(0.0f, 0.0f, 1.0f);
	camera.isPerspective = false;
	camera.size = make_vector(300U, 300U);
	setScene(camera, new PieceRenderData(piece));
}

const QPixmap* PieceLibraryWidget :: dragPixmap()
{
	return pixmap();
}

bool PieceLibraryWidget :: isDraggable()
{
	return false;
}

GlassMime::Type PieceLibraryWidget :: mimeType()
{
	return GlassMime::PIECELIBRARY_MIME;
}

