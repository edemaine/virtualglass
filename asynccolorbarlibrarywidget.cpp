
#include "asynccolorbarlibrarywidget.h"

AsyncColorBarLibraryWidget :: AsyncColorBarLibraryWidget(GlassColor* _glassColor, QWidget* _parent): AsyncRenderWidget(_parent) 
{
        setFixedSize(100, 100);
        setAttribute(Qt::WA_LayoutUsesWidgetRect);

        setGraphicsEffect(new QGraphicsHighlightEffect());
        connect(graphicsEffect(), SIGNAL(enabledChanged(bool)), graphicsEffect(), SLOT(setStyleSheet(bool)));
        connect(graphicsEffect(), SIGNAL(styleSheetString(QString)), this, SLOT(setStyleSheet(QString)));

	glassColor = _glassColor;

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
	// update the drag pixmap in the main thread, since it's fast
	Color c = *(glassColor->getColor());
        QPixmap _dragPixmap(100, 100);
        _dragPixmap.fill(QColor(255*c.r, 255*c.g, 255*c.b, 255*MAX(0.1, c.a)));
	dragPixmap = _dragPixmap;
}

const QPixmap* AsyncColorBarLibraryWidget :: getDragPixmap()
{
        return &(this->dragPixmap);
}

void AsyncColorBarLibraryWidget :: updatePixmaps()
{
	updateDragPixmap();

	//queue up an async update:
	Camera camera;
	camera.eye = make_vector(0.0f, 11.0f, 5.0f);
	camera.lookAt = make_vector(0.0f, 0.0f, 5.0f);
	camera.up = make_vector(0.0f, 0.0f, 1.0f);
	camera.isPerspective = false;
	camera.size = make_vector(300U, 300U);
	setScene(camera, new GlassColorRenderData(glassColor));
}

