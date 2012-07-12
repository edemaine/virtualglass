
#include "purecolorlibrarywidget.h"

PureColorLibraryWidget :: PureColorLibraryWidget(Color color, QString colorName, QWidget* parent): QLabel(parent)
{
	setFixedSize(300, 40);
	setScaledContents(true);
        setAttribute(Qt::WA_LayoutUsesWidgetRect);

	isSelected = false;
	this->colorName = colorName;
	this->color = color;

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
        painter.fillRect(QRect(10, 10, 20, 20), QBrush(QColor(255*color.r, 255*color.g, 255*color.b, 255*color.a)));
        painter.drawRect(QRect(10, 10, 20, 20));
        painter.drawText(QPointF(40, 25), colorName);

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

void PureColorLibraryWidget :: setAlpha(float a)
{
	this->color.a = a;
}

Color PureColorLibraryWidget :: getColor()
{
	return color;
}

QString PureColorLibraryWidget :: getColorName()
{
	return colorName;
}


