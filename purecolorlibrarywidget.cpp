
#include <QPainter>
#include <QPen>
#include "purecolorlibrarywidget.h"
#include "glasscolor.h"

PureColorLibraryWidget :: PureColorLibraryWidget(GlassColor* _color, QWidget* parent): QLabel(parent)
{
	setFixedSize(300, 40);
	setScaledContents(true);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);

	isSelected = false;
	this->color = _color;

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
	Color c = color->getColor();
	painter.fillRect(QRect(10, 10, 20, 20), QBrush(QColor(255*c.r, 255*c.g, 255*c.b, 255*c.a)));
	painter.drawRect(QRect(10, 10, 20, 20));
	painter.drawText(QPointF(40, 25), color->getLongName().c_str());

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

GlassColor* PureColorLibraryWidget :: getGlassColorCopy()
{
	return deep_copy(color);
}

Color PureColorLibraryWidget :: getColor()
{
	return color->getColor();
}

string PureColorLibraryWidget :: getShortName()
{
	return color->getShortName();
}

string PureColorLibraryWidget :: getLongName()
{
	return color->getLongName();
}

