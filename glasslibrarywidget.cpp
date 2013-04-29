
#include "glasslibrarywidget.h"
#include "globalbackgroundcolor.h"

GlassLibraryWidget :: GlassLibraryWidget(QWidget* _parent) : QLabel(_parent)
{
	setDependancy(false, IS_DEPENDANCY);
}

void GlassLibraryWidget :: updateStyleSheet() 
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
		color = GlobalBackgroundColor::qcolor;
		style.append("border: 2px solid " + color.name() + ";");
	}

	setStyleSheet(style);
}

void GlassLibraryWidget::setDependancy(bool _hd, enum Dependancy _d)
{
	hasDependancy = _hd;
	dependancy = _d;
	updateStyleSheet();
}



