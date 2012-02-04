
#include "purecolorlibrarywidget.h"

PureColorLibraryWidget :: PureColorLibraryWidget(Color color, QString colorName, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(300, 40);
	setScaledContents(true);
	setMouseTracking(true);

	QPixmap pixmap(300, 40);
	QPainter painter(&pixmap);
	painter.fillRect(pixmap.rect(), QBrush(Qt::white));
	painter.fillRect(QRect(10, 10, 20, 20), QBrush(QColor(255*color.r, 255*color.g, 255*color.b, 255*color.a)));
	painter.drawRect(QRect(10, 10, 20, 20));
	painter.drawText(QPointF(40, 25), colorName);
	painter.end();

	setPixmap(pixmap);
        setAttribute(Qt::WA_LayoutUsesWidgetRect);
	this->colorName = colorName;

	this->color = color;
        setGraphicsEffect(new QGraphicsHighlightEffect());
        connect(graphicsEffect(), SIGNAL(enabledChanged(bool)), graphicsEffect(), SLOT(setStyleSheet(bool)));
        connect(graphicsEffect(), SIGNAL(styleSheetString(QString)), this, SLOT(setStyleSheet(QString)));
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


