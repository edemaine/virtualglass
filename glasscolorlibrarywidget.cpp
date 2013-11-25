
#include "constants.h"
#include "glasscolor.h"
#include "canerenderdata.h"
#include "glasscolorlibrarywidget.h"
#include "canetemplate.h"
#include "cane.h"

GlassColorLibraryWidget :: GlassColorLibraryWidget(GlassColor* _glassColor, MainWindow* _window, 
	Cane* _circleCane, Cane* _squareCane) : AsyncRenderWidget(_window), glassColor(_glassColor), 
	circleCane(_circleCane), squareCane(_squareCane) 
{
	setFixedSize(100, 100);
	if (_circleCane == NULL)
		circleCane = new Cane(CaneTemplate::BASE_CIRCLE);
	circleCane->setOutermostCasingColor(_glassColor);
	if (_squareCane == NULL)
		squareCane = new Cane(CaneTemplate::BASE_SQUARE);
	squareCane->setOutermostCasingColor(_glassColor);
	updatePixmaps();
	connect(this->glassColor, SIGNAL(modified()), this, SLOT(updatePixmaps()));
}

void GlassColorLibraryWidget :: paintEvent(QPaintEvent *event)
{
	AsyncRenderWidget::paintEvent(event);

	QPainter painter;
	painter.begin(this);
	painter.setPen(QPen(Qt::black));
	painter.drawText(rect().adjusted(3, 0, 3, 0), Qt::AlignBottom | Qt::AlignLeft, glassColor->shortName().c_str());
	painter.end();
}

void GlassColorLibraryWidget :: updateDragPixmap()
{
	QPixmap _pixmap(200, 200);
	_pixmap.fill(Qt::transparent);
	QPainter painter(&_pixmap);
	
	// Fill in with color
	Color c = glassColor->color();
	QColor qc(255*c.r, 255*c.g, 255*c.b, MAX(255*c.a, 20));
	painter.setBrush(qc);
	painter.setPen(Qt::NoPen);
	painter.drawEllipse(10, 10, 180, 180);
	painter.end();

	_dragPixmap = _pixmap.scaled(100, 100);
}

const QPixmap* GlassColorLibraryWidget :: dragPixmap()
{
	return &(this->_dragPixmap);
}

bool GlassColorLibraryWidget :: isDraggable()
{
	return true;
}

GlassMime::Type GlassColorLibraryWidget :: mimeType()
{
	return GlassMime::COLOR_LIBRARY_MIME;
}

void GlassColorLibraryWidget :: updatePixmaps()
{
	updateDragPixmap();

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






