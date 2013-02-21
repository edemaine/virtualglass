

#include "asyncpiecelibrarywidget.h"

AsyncPieceLibraryWidget :: AsyncPieceLibraryWidget(Piece* _piece, QWidget* _parent): AsyncRenderWidget(_parent), piece(_piece)
{
	setFixedSize(100, 100);
	eyePosition.x = -16.0;
	eyePosition.y = 0.0;
	eyePosition.z = 0.0;
	updatePixmap();
}

void AsyncPieceLibraryWidget :: updateEyePosition(Vector3f _eyePosition)
{
	eyePosition = _eyePosition;
}

void AsyncPieceLibraryWidget :: updatePixmap()
{
	// indicate to the user that the image is being updated
	// busy-ness is turned off in inherited AsyncRenderWidget::renderFinished()
	setBusy(true);

	//queue up an async update:
	Camera camera;
	camera.eye = eyePosition;
	camera.lookAt = make_vector(0.0f, 0.0f, 0.0f);
	camera.up = make_vector(0.0f, 0.0f, 1.0f);
	camera.isPerspective = false;
	camera.size = make_vector(300U, 300U);
	setScene(camera, new PieceRenderData(piece));
}

