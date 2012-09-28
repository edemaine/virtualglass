#include "libraryitem.h"


LibraryItem :: LibraryItem(QWidget* _parent) : QLabel(_parent)
{
	isBusy = false;
	hasDependancy = false;
}

void LibraryItem :: updateStyleSheet() 
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

	setStyleSheet(style);
}

void LibraryItem::setBusy(bool _b)
{
	isBusy = _b;
	updateStyleSheet();
}

void LibraryItem::setDependancy(bool _hd, enum Dependancy _d)
{
	hasDependancy = _hd;
	dependancy = _d;
	updateStyleSheet();
}



