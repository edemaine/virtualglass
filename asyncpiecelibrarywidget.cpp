
#include "asyncpiecelibrarywidget.h"
#include "qgraphicshighlighteffect.h"
#include "piecerenderdata.h"

AsyncPieceLibraryWidget :: AsyncPieceLibraryWidget(Piece* _piece, QWidget* parent): AsyncRenderWidget(parent), piece(_piece)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

	setAttribute(Qt::WA_LayoutUsesWidgetRect);

	setGraphicsEffect(new QGraphicsHighlightEffect());
	connect(graphicsEffect(),SIGNAL(enabledChanged(bool)),graphicsEffect(),SLOT(setStyleSheet(bool)));
	connect(graphicsEffect(),SIGNAL(styleSheetString(QString)),this,SLOT(setStyleSheet(QString)));
}

Piece* AsyncPieceLibraryWidget :: getPiece()
{
	return piece;
}

void AsyncPieceLibraryWidget :: updatePixmap(Vector3f eye_loc)
{
	//queue up an async update:
	Camera camera;
	camera.eye = eye_loc;
	camera.lookAt = make_vector(0.0f, 0.0f, 0.0f);
	camera.up = make_vector(0.0f, 0.0f, 1.0f);
	camera.isPerspective = false;
	camera.size = make_vector(300U, 300U);
	setScene(camera, new PieceRenderData(piece));

}

