
#include <QPainter>
#include <QPen>
#include <QHBoxLayout>
#include <QMouseEvent>

#include "purecolorlibrarywidget.h"
#include "glasscolor.h"

PureColorLibraryWidget :: PureColorLibraryWidget(GlassColor* __color, 
	ColorEditorWidget* editor, QWidget* parent): QLabel(parent)
{
	setFixedSize(300, 40);
	this->editor = editor;
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
	painter.fillRect(QRect(0, 0, 20, 20), QBrush(QColor(255*c.r, 255*c.g, 255*c.b, 255*c.a)));
	painter.drawRect(QRect(0, 0, 19, 19));
	swatch->setPixmap(pixmap);
	layout->addWidget(swatch, 0);

	QLabel* nameLabel = new QLabel(_color->longName().c_str(), this);
	nameLabel->setPalette(QPalette(Qt::black));
	layout->addWidget(nameLabel, 1);

	renderPixmap();

	clickDown = false;
}

void PureColorLibraryWidget :: mousePressEvent(QMouseEvent*)
{
	clickDown = true;
}

void PureColorLibraryWidget :: mouseMoveEvent(QMouseEvent* event)
{
	if (!this->rect().contains(event->pos()))
	{
		clickDown = false;
		event->ignore();
	}
}

void PureColorLibraryWidget :: mouseReleaseEvent(QMouseEvent* event)
{
	if (clickDown)
		this->editor->setGlassColorProperties(this->_color);	
	else
		event->ignore();
	clickDown = false;
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
	painter.fillRect(0, 0, 300, 40, Qt::white);
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

