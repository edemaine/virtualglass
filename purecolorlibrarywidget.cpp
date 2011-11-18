
#include "purecolorlibrarywidget.h"

PureColorLibraryWidget :: PureColorLibraryWidget(Color color, char* /*name*/, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

	QImage image("./checkerboard.png");

        QPainter painter(&image);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.fillRect(image.rect(), QBrush(QColor(255*color.r, 255*color.g, 255*color.b, 255*color.a)));
        painter.end();

	setPixmap(QPixmap::fromImage(image.scaled(100, 100)));
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


