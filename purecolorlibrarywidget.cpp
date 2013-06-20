
#include <QPainter>
#include <QPen>
#include "purecolorlibrarywidget.h"
#include "glasscolor.h"

PureColorLibraryWidget :: PureColorLibraryWidget(GlassColor* __color, QWidget* parent): QLabel(parent)
{
	setFixedSize(300, 40);
	setScaledContents(true);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);

	isSelected = false;
	this->_color = __color;

	renderPixmap();
}

void PureColorLibraryWidget :: setSelected(bool s)
{
	if (s != isSelected)
	{
		isSelected = s;
		renderPixmap();
	}
}

void PureColorLibraryWidget :: renderPixmap()
{
	QPixmap pixmap(300, 40);
	QPainter painter(&pixmap);
	painter.fillRect(pixmap.rect(), QBrush(Qt::white));
	Color c = _color->color();
	painter.fillRect(QRect(10, 10, 20, 20), QBrush(QColor(255*c.r, 255*c.g, 255*c.b, 255*c.a)));
	painter.drawRect(QRect(10, 10, 20, 20));
	painter.drawText(QPointF(40, 25), _color->longName().c_str());

	// do highlighting if selected
	if (isSelected)
	{
		QPen pen;
		pen.setWidth(2);
		pen.setColor(Qt::blue);
		painter.setPen(pen);
		painter.drawRect(1, 1, 298, 38);
	}

	painter.end();

	setPixmap(pixmap);
}

GlassColor* PureColorLibraryWidget :: glassColorCopy()
{
	return deep_copy(_color);
}

Color PureColorLibraryWidget :: color()
{
	return _color->color();
}

string PureColorLibraryWidget :: shortName()
{
	return _color->shortName();
}

string PureColorLibraryWidget :: longName()
{
	return _color->longName();
}

