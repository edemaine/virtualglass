
#include "asyncpiecelibrarywidget.h"
#include "qgraphicshighlighteffect.h"
#include "piecerenderdata.h"

AsyncPieceLibraryWidget :: AsyncPieceLibraryWidget(Piece* _piece, QWidget* parent): AsyncRenderWidget(parent), piece(_piece)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
	eyePosition.x = -16.0;
	eyePosition.y = 0.0;
	eyePosition.z = 0.0;

	setGraphicsEffect(new QGraphicsHighlightEffect());
	connect(graphicsEffect(),SIGNAL(styleSheetString(QString)),this,SLOT(setStyleSheet(QString)));
}

Piece* AsyncPieceLibraryWidget :: getPiece()
{
	return piece;
}

void AsyncPieceLibraryWidget :: updateEyePosition(Vector3f _eyePosition)
{
	eyePosition = _eyePosition;
}

void AsyncPieceLibraryWidget :: updatePixmap()
{
        // indicate to the user that the image is being updated
        // busy-ness is turned off inherited AsyncRenderWidget::renderFinished()
	static_cast<QGraphicsHighlightEffect*>(graphicsEffect())->setBusy(true);

	//queue up an async update:
	Camera camera;
	camera.eye = eyePosition;
	camera.lookAt = make_vector(0.0f, 0.0f, 0.0f);
	camera.up = make_vector(0.0f, 0.0f, 1.0f);
	camera.isPerspective = false;
	camera.size = make_vector(300U, 300U);
	setScene(camera, new PieceRenderData(piece));
}

