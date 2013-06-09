
#include "glasslibrarywidget.h"
#include "globalbackgroundcolor.h"

GlassLibraryWidget :: GlassLibraryWidget(QWidget* _parent) : QLabel(_parent)
{
	setDependancy(NO_DEPENDANCY);
}

void GlassLibraryWidget :: updateStyleSheet() 
{
	QColor color;
	QString style;
	
	switch (dependancy)
	{
		case NO_DEPENDANCY:	
			color = GlobalBackgroundColor::qcolor;
			style.append("border: 2px solid " + color.name() + ";");
			break;
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

	setStyleSheet(style);
}

void GlassLibraryWidget::setDependancy(enum Dependancy _d)
{
	dependancy = _d;
	updateStyleSheet();
}



