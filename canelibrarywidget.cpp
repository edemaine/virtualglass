
#include "glassmime.h"
#include "cane.h"
#include "glasscolor.h"
#include "canerenderdata.h"
#include "canelibrarywidget.h"
#include "canecrosssectionrender.h"

CaneLibraryWidget :: CaneLibraryWidget(Cane *_pullPlan, MainWindow *_window) 
	: AsyncRenderWidget(_window), pullPlan(_pullPlan)
{
	setFixedSize(100, 100);
	updatePixmaps();
}

void CaneLibraryWidget :: updatePixmaps()
{
	// This is fast enough to do in real time
	updateDragPixmap();

	// queue up an async update:
	Camera camera;
	camera.eye = make_vector(0.0f, 11.0f, 5.0f);
	camera.lookAt = make_vector(0.0f, 0.0f, 5.0f);
	camera.up = make_vector(0.0f, 0.0f, 1.0f);
	camera.isPerspective = false;
	camera.size = make_vector(300U, 300U);
	setScene(camera, new CaneRenderData(pullPlan));
}

const QPixmap* CaneLibraryWidget :: dragPixmap()
{
	return &(this->_dragPixmap);
}

bool CaneLibraryWidget :: isDraggable()
{
	return true;
}

GlassMime::Type CaneLibraryWidget :: mimeType()
{
	return GlassMime::PULLPLAN_LIBRARY_MIME;
}

void CaneLibraryWidget :: updateDragPixmap()
{
	QPixmap _pixmap(100, 100);
	_pixmap.fill(Qt::transparent); 

	QPainter painter(&_pixmap);
	CaneCrossSectionRender::render(&painter, 100, pullPlan);
	painter.end();

	_dragPixmap = _pixmap;
}


