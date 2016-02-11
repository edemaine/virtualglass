
#include <QPainter>
#include <QPen>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QApplication>

#include "purecolorlibrarywidget.h"
#include "glasscolor.h"
#include "globalbackgroundcolor.h"

PureColorLibraryWidget :: PureColorLibraryWidget(GlassColor* __color, 
	QWidget* parent): QLabel(parent)
{
	setFixedSize(300, 40);
	this->_color = __color;
	isSelected = false;

	QHBoxLayout* layout = new QHBoxLayout(this);
	this->setLayout(layout);
	layout->setContentsMargins(10, 0, 0, 0);

	swatch = new QLabel(this);
	swatch->setFixedSize(20, 20);
	QPixmap pixmap(20, 20);
	QPainter painter(&pixmap);

	Color c = _color->color();
	QColor qc;
	qc.setRgbF(c.r, c.g, c.b, c.a);
	painter.fillRect(QRect(0, 0, 20, 20), QBrush(qc));
	painter.drawRect(QRect(0, 0, 19, 19));
	swatch->setPixmap(pixmap);
	layout->addWidget(swatch, 0);

	QLabel* nameLabel = new QLabel(_color->longName().c_str(), this);
	nameLabel->setPalette(QPalette(Qt::black));
	layout->addWidget(nameLabel, 1);

	renderPixmap();

	mouseDown = false;
}

void PureColorLibraryWidget :: mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		mouseDown = true;
		mouseDownPos = event->pos();
	}
}

void PureColorLibraryWidget :: mouseMoveEvent(QMouseEvent* event)
{
	if ((event->pos() - mouseDownPos).manhattanLength() > QApplication::startDragDistance()
		|| !this->rect().contains(event->pos()))
	{
		mouseDown = false;
		event->ignore();
	}
}

void PureColorLibraryWidget :: mouseReleaseEvent(QMouseEvent* event)
{
	if (mouseDown)
		emit colorSelected(this->_color);
	else
		event->ignore();
	mouseDown = false;
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
	painter.fillRect(0, 0, 300, 40, GlobalBackgroundColor::qcolor);
	if (isSelected) // do highlighting if selected
	{
		QPen pen;
		pen.setColor(QColor(0, 0, 255));
		pen.setWidth(2);
		painter.setPen(pen);
		painter.drawRect(1, 1, 298, 38);
	}
	painter.end();

	this->setPixmap(pixmap);
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

