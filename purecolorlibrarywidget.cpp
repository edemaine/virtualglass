
#include "purecolorlibrarywidget.h"

PureColorLibraryWidget :: PureColorLibraryWidget(Color color, char* name, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(150, 40);
	setScaledContents(true);
	setMouseTracking(true);

	QPixmap pixmap(150, 40);
	QPainter painter(&pixmap);
	painter.fillRect(pixmap.rect(), QBrush(Qt::white));
	painter.fillRect(QRect(10, 10, 20, 20), QBrush(QColor(255*color.r, 255*color.g, 255*color.b, 255*color.a)));
	painter.drawText(QPointF(40, 24), name);
	painter.end();

	setPixmap(pixmap);
        setAttribute(Qt::WA_LayoutUsesWidgetRect);

	this->color = color;
}

void PureColorLibraryWidget :: setAlpha(float a)
{
	this->color.a = a;
}

Color PureColorLibraryWidget :: getColor()
{
	return color;
}


