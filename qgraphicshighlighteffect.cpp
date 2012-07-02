#include "qgraphicshighlighteffect.h"

QGraphicsHighlightEffect::QGraphicsHighlightEffect(QObject* parent) : QGraphicsEffect(parent)
{
}

QRectF QGraphicsHighlightEffect::boundingRectFor( const QRectF &sourceRect) const
{
	return sourceRect.adjusted(0, 0, 0, 0); 
}

void QGraphicsHighlightEffect :: setStyleSheet(bool enableBorder)
{
	QColor color;

	if (enableBorder)
	{
		switch (dependancy)
		{
			case IS_DEPENDANCY:
				color = QColor(0, 0, 255, 255);	
				emit styleSheetString("border: 2px solid " + color.name() + ";");
				break;
			case USES_DEPENDANCY:
				color = QColor(200, 100, 0, 255);
				emit styleSheetString("border: 2px dotted " + color.name() + ";");
				break;
			case IS_USED_BY_DEPENDANCY:
				color = QColor(0, 139, 69, 255);
				emit styleSheetString("border: 2px dashed " + color.name() + ";");
				break;
		}
	}
	else
	{
		color = QColor(200, 200, 200, 0);
		emit styleSheetString("border: 2px solid " + color.name());
	}
}

void QGraphicsHighlightEffect::setHighlightType(int dependancy)
{
	this->dependancy = dependancy;
}

void QGraphicsHighlightEffect::draw( QPainter *painter )
{
	drawSource(painter);
	return;
}
