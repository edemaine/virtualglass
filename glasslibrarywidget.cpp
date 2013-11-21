
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QMimeData>
#include <QDrag>

#include "glassmime.h"
#include "glasscolorlibrarywidget.h"
#include "canelibrarywidget.h"
#include "glasslibrarywidget.h"
#include "globalbackgroundcolor.h"
#include "mainwindow.h"

GlassLibraryWidget :: GlassLibraryWidget(MainWindow* _window) : QLabel(_window)
{
	this->window = _window;
	setDependancy(NO_DEPENDANCY);
}

const QPixmap* GlassLibraryWidget :: dragPixmap()
{
	return pixmap();
}

bool GlassLibraryWidget :: isDraggable()
{
	return false;
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

void GlassLibraryWidget :: setDependancy(enum Dependancy _d)
{
	dependancy = _d;
	updateStyleSheet();
}

int GlassLibraryWidget :: hitValue(QPoint p)
{
	if (0 < p.x() && p.x() < 20 && 0 < p.y() && p.y() < 20)
		return 1;
	else if (80 < p.x() && p.x() < 100 && 0 < p.y() && p.y() < 20)
		return -1;
	else 
		return 0;
}

void GlassLibraryWidget :: mousePressEvent(QMouseEvent* event)
{
        if (event->button() == Qt::LeftButton)
        {
                clickDown = true;
                clickDownPos = event->pos();
        }	
}

void GlassLibraryWidget :: mouseMoveEvent(QMouseEvent* event)
{
        if ((event->pos() - clickDownPos).manhattanLength() > QApplication::startDragDistance()
                || !this->rect().contains(event->pos()))
	{
		char buf[500];
		QPixmap pixmap;

		if (!this->isDraggable())
		{
			clickDown = false;
			return;
		}

		GlassMime::encode(buf, this, this->mimeType());

                QByteArray pointerData(buf);
                QMimeData* mimeData = new QMimeData;
                mimeData->setText(pointerData);

                QDrag *drag = new QDrag(this);
                drag->setMimeData(mimeData);
                drag->setPixmap(*(this->dragPixmap()));
                drag->setHotSpot(QPoint(50, 50));

                drag->exec(Qt::CopyAction);
	}
}

void GlassLibraryWidget :: mouseReleaseEvent(QMouseEvent*)
{
	if (clickDown)
	{
		int v = hitValue(clickDownPos);
		if (v > 0)
			this->window->copyLibraryWidget(this);
		else if (v < 0)
			this->window->deleteLibraryWidget(this);
		else
			this->window->setEditorLibraryWidget(this);
	}

	clickDown = false;
}

void GlassLibraryWidget :: paintEvent(QPaintEvent* event)
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
                
		if (!this->window->hasDependancies(this))
		{
			color.setRgb(255, 255, 255);
			painter.fillRect(100-3-15, 3, 15, 15, color);
			painter.drawLine(100-6, 10, 100-16, 10);
		}
        }
        painter.end();
}

