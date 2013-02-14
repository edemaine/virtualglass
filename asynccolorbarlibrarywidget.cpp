
#include "constants.h"
#include "glasscolor.h"
#include "pullplanrenderdata.h"
#include "asynccolorbarlibrarywidget.h"
#include "pulltemplate.h"
#include "pullplan.h"

AsyncColorBarLibraryWidget :: AsyncColorBarLibraryWidget(GlassColor* _glassColor, QWidget* _parent, 
	PullPlan* _circlePlan, PullPlan* _squarePlan) : AsyncRenderWidget(_parent), glassColor(_glassColor), 
	circlePlan(_circlePlan), squarePlan(_squarePlan) 
{
	setFixedSize(100, 100);
	if (_circlePlan == NULL)
		circlePlan = new PullPlan(PullTemplate::BASE_CIRCLE);
	circlePlan->setOutermostCasingColor(_glassColor);
	if (_squarePlan == NULL)
		squarePlan = new PullPlan(PullTemplate::BASE_SQUARE);
	squarePlan->setOutermostCasingColor(_glassColor);
	updatePixmaps();
}

void AsyncColorBarLibraryWidget :: paintEvent(QPaintEvent *event)
{
	AsyncRenderWidget::paintEvent(event);
	QPainter painter(this);
	painter.setPen(QPen(Qt::black));
	painter.drawText(rect().adjusted(5, 0, 0, -2), Qt::AlignBottom | Qt::AlignLeft, glassColor->getShortName().c_str());
}

void AsyncColorBarLibraryWidget :: updateDragPixmap()
{
        QPixmap _pixmap(200, 200);
        _pixmap.fill(Qt::transparent);
        QPainter painter(&_pixmap);
	
	// Fill in with color
	Color c = glassColor->getColor();
	QColor qc(255*c.r, 255*c.g, 255*c.b, MAX(255*c.a, 20));
	painter.setBrush(qc);
        painter.setPen(Qt::NoPen);
	painter.drawEllipse(10, 10, 180, 180);
        painter.end();

        dragPixmap = _pixmap.scaled(100, 100);
}

const QPixmap* AsyncColorBarLibraryWidget :: getDragPixmap()
{
	return &(this->dragPixmap);
}

void AsyncColorBarLibraryWidget :: updatePixmaps()
{
	updateDragPixmap();

	// indicate to the user that the image is being updated
	// busy-ness is turned off inherited AsyncRenderWidget::renderFinished()
	setBusy(true);

	//queue up an async update (though as of 9/12 the rendering
	//is real-time anyway):
	Camera camera;
	camera.eye = make_vector(0.0f, 11.0f, 5.0f);
	camera.lookAt = make_vector(0.0f, 0.0f, 5.0f);
	camera.up = make_vector(0.0f, 0.0f, 1.0f);
	camera.isPerspective = false;
	camera.size = make_vector(300U, 300U);
	setScene(camera, new GlassColorRenderData(glassColor));
}


