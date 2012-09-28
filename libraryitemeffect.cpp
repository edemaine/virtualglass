#include "libraryitemeffect.h"

LibraryItemEffect::LibraryItemEffect(QObject* parent) : QGraphicsEffect(parent)
{
	isBusy = false;
	hasDependancy = false;
}

QRectF LibraryItemEffect::boundingRectFor( const QRectF &sourceRect) const
{
	return sourceRect;
}

void LibraryItemEffect :: setStyleSheet()
{
	QColor color;

	QString style;
	
	if (hasDependancy)
	{
		switch (dependancy)
		{
			case IS_DEPENDANCY:
				color = QColor(0, 0, 255);	
				style.append("border: 2px solid " + color.name() + ";");
				break;
			case USES_DEPENDANCY:
				color = QColor(200, 100, 0);
				style.append("border: 2px dotted " + color.name() + ";");
				break;
			case USEDBY_DEPENDANCY:
				color = QColor(0, 139, 69);
				style.append("border: 2px dashed " + color.name() + ";");
				break;
		}
	}
	else
	{
		color = QColor(200, 200, 200);
		style.append("border: 2px solid " + color.name() + ";");
	}

	if (isBusy)
	{
		color = QColor(0, 0, 0); 
		style.append("background-color: " + color.name() + ";");
	}
	else 
	{
		color = QColor(200, 200, 200);
		style.append("background-color: " + color.name() + ";");
	}	

	emit styleSheetString(style);
}

void LibraryItemEffect::setBusy(bool _b)
{
	isBusy = _b;
	setStyleSheet();
}

void LibraryItemEffect::setDependancy(bool _hd, enum Dependancy _d)
{
	hasDependancy = _hd;
	dependancy = _d;
	setStyleSheet();
}

void LibraryItemEffect::draw( QPainter *painter )
{
#ifdef UNDEF
	QPoint offset;

	const QPixmap pixmap = sourcePixmap(Qt::DeviceCoordinates, &offset);

	painter->setWorldTransform(QTransform());
	painter->drawPixmap(offset, pixmap); 

     QPoint offset;
     if (sourceIsPixmap()) {
         // No point in drawing in device coordinates (pixmap will be scaled anyways).
         const QPixmap pixmap = sourcePixmap(Qt::LogicalCoordinates, &offset);
     
         painter->drawPixmap(offset, pixmap);
     } else {
         // Draw pixmap in device coordinates to avoid pixmap scaling;
         const QPixmap pixmap = sourcePixmap(Qt::DeviceCoordinates, &offset);
         painter->setWorldTransform(QTransform());
    
         painter->drawPixmap(offset, pixmap);
     }
#endif
	
	painter->fillRect(0, 0, 50, 50, QColor(0, 0, 129));
	drawSource(painter);
}


