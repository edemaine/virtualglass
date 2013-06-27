

#include "pullplan.h"
#include "glasscolor.h"
#include "pullplanrenderdata.h"
#include "pullplanlibrarywidget.h"
#include "pullplancrosssectionrender.h"

PullPlanLibraryWidget :: PullPlanLibraryWidget(PullPlan *_pullPlan, MainWindow *_window) 
	: AsyncRenderWidget(_window), pullPlan(_pullPlan)
{
	setFixedSize(100, 100);
	updatePixmaps();
}

void PullPlanLibraryWidget :: updatePixmaps()
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
	setScene(camera, new PullPlanRenderData(pullPlan));
}

const QPixmap* PullPlanLibraryWidget :: getDragPixmap()
{
	return &(this->dragPixmap);
}

void PullPlanLibraryWidget :: updateDragPixmap()
{
	QPixmap _pixmap(100, 100);
	_pixmap.fill(Qt::transparent); 

	QPainter painter(&_pixmap);
	PullPlanCrossSectionRender::render(&painter, 100, pullPlan);
	painter.end();

	dragPixmap = _pixmap;
}


