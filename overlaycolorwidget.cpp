
#include "overlaycolorwidget.h"

OverlayColorWidget :: OverlayColorWidget(Color** color, QWidget* parent): QWidget(parent)
{
        setAcceptDrops(true);
	setFixedSize(20, 20);

	this->color = color;
}

void OverlayColorWidget :: paintEvent(QPaintEvent*)
{
	QPainter painter;
	painter.begin(this);
        painter.fillRect(QRect(0, 0, 20, 20), QColor(0, 0, 0, 255));
        painter.fillRect(QRect(1, 1, 18, 18), QColor(255, 255, 255, 255));
	Color* c = (*(this->color));
        painter.fillRect(QRect(1, 1, 18, 18), QColor(255*c->r, 255*c->g, 255*c->b, 255*c->a));
        painter.end();
}

void OverlayColorWidget :: dragEnterEvent(QDragEnterEvent* event)
{
        event->acceptProposedAction();
}

void OverlayColorWidget :: dropEvent(QDropEvent* event) 
{
        PullPlan* droppedPlan;
        int type;
        sscanf(event->mimeData()->text().toAscii().constData(), "%p %d", &droppedPlan, &type);		

	if (type == COLOR_BAR_MIME) 
	{
		event->accept();
		*(this->color) = droppedPlan->getOutermostCasingColor();
		emit colorChanged();
	}
}




