
#include "constants.h"
#include "glasscolor.h"
#include "pullplanrenderdata.h"
#include "asynccolorbarlibrarywidget.h"

AsyncColorBarLibraryWidget :: AsyncColorBarLibraryWidget(GlassColor* _glassColor, QWidget* _parent): AsyncRenderWidget(_parent), glassColor(_glassColor) 
{
	setFixedSize(100, 100);
	updatePixmaps();
}

GlassColor* AsyncColorBarLibraryWidget :: getGlassColor()
{
	return glassColor;
}

void AsyncColorBarLibraryWidget :: paintEvent(QPaintEvent *event)
{
	AsyncRenderWidget::paintEvent(event);
	QPainter painter(this);
	painter.setPen(QPen(Qt::black));
	painter.drawText(rect().adjusted(5, 0, 0, -2), Qt::AlignBottom | Qt::AlignLeft, *(glassColor->getName()));
}

void AsyncColorBarLibraryWidget :: updateDragPixmap()
{
        QPixmap _pixmap(200, 200);
        _pixmap.fill(Qt::transparent);
        QPainter painter(&_pixmap);
	
	// Fill in with color
	Color* c = glassColor->getColor();
	QColor qc(255*c->r, 255*c->g, 255*c->b, MAX(255*c->a, 20));
	painter.setBrush(qc);
        painter.setPen(Qt::NoPen);
	painter.drawEllipse(10, 10, 180, 180);

	/*
	// Draw the little outer boundary line
	QPen pen;
	pen.setWidth(3);
	pen.setColor(Qt::black);
	painter.setPen(pen);
	painter.drawEllipse(10, 10, 180, 180);
	*/

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


