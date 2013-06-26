
#include <QPainter>
#include <QMouseEvent>

#include "glasslibrarywidget.h"
#include "globalbackgroundcolor.h"
#include "mainwindow.h"

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

int GlassLibraryWidget::hitValue(QPoint p)
{
	if (0 < p.x() && p.x() < 20 && 0 < p.y() && p.y() < 20)
		return 1;
	else if (80 < p.x() && p.x() < 100 && 0 < p.y() && p.y() < 20)
		return -1;
	else 
		return 0;
}

void GlassLibraryWidget::mouseReleaseEvent(QMouseEvent* event)
{
	MainWindow* m = static_cast<MainWindow*>(this->window());
        int v = hitValue(event->pos());
        if (v > 0)
                m->copyLibraryWidget(this);
        else if (v < 0)
                m->deleteLibraryWidget(this);
        else
		m->setEditorLibraryWidget(this);
}

void GlassLibraryWidget::paintEvent(QPaintEvent* event)
{
	QLabel::paintEvent(event);

        QPainter painter;
        painter.begin(this);
        painter.setPen(QPen(Qt::black));
        if (this->dependancy == IS_DEPENDANCY)
        {
                QColor color;
                color.setRgb(255, 255, 255);
                painter.fillRect(3, 3, 15, 15, color);
                painter.drawLine(10, 5, 10, 15);
                painter.drawLine(5, 10, 15, 10); 
                
		if (static_cast<MainWindow*>(this->window())->hasNoDependancies(this))
		{
			color.setRgb(255, 255, 255);
			painter.fillRect(100-3-15, 3, 15, 15, color);
			painter.drawLine(100-6, 10, 100-16, 10);
		}
        }
        painter.end();
}

