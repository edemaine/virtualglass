
#include "purecolorlibrarywidget.h"

PureColorLibraryWidget :: PureColorLibraryWidget(Color color, QString name, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(250, 40);
	setScaledContents(true);
	setMouseTracking(true);

	QPixmap pixmap(250, 40);
	QPainter painter(&pixmap);
	painter.fillRect(pixmap.rect(), QBrush(Qt::white));
	painter.fillRect(QRect(10, 10, 20, 20), QBrush(QColor(255*color.r, 255*color.g, 255*color.b, 255*color.a)));
	painter.drawRect(QRect(10, 10, 20, 20));
	painter.drawText(QPointF(40, 25), name);
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


