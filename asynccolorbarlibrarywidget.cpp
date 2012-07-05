
#include "asynccolorbarlibrarywidget.h"
#include "pullplanrenderdata.h"

AsyncColorBarLibraryWidget :: AsyncColorBarLibraryWidget(PullPlan* _plan, QWidget* _parent): AsyncPullPlanLibraryWidget(_plan, _parent)
{
}

void AsyncColorBarLibraryWidget :: paintEvent(QPaintEvent *event)
{
	AsyncPullPlanLibraryWidget::paintEvent(event);
	QPainter painter(this);
	painter.setPen(QPen(Qt::black));
	painter.drawText(rect().adjusted(5, 0, 0, -2), Qt::AlignBottom | Qt::AlignLeft, pullPlan->getName());
}

void AsyncColorBarLibraryWidget :: updatePixmaps()
{
	// update the drag pixmap in the main thread, since it's fast
	Color c = *(getPullPlan()->getOutermostCasingColor());
        QPixmap _dragPixmap(100, 100);
        _dragPixmap.fill(QColor(255*c.r, 255*c.g, 255*c.b, 255*MAX(0.1, c.a)));
	dragPixmap = _dragPixmap;

	//queue up an async update:
	Camera camera;
	camera.eye = make_vector(0.0f, 11.0f, 5.0f);
	camera.lookAt = make_vector(0.0f, 0.0f, 5.0f);
	camera.up = make_vector(0.0f, 0.0f, 1.0f);
	camera.isPerspective = false;
	camera.size = make_vector(300U, 300U);
	setScene(camera, new ColorBarRenderData(pullPlan));
}

