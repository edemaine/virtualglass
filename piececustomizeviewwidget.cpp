
#include <QResizeEvent>
#include <QPainter>
#include "piececustomizeviewwidget.h"
#include "piece.h"
#include "constants.h"

PieceCustomizeViewWidget :: PieceCustomizeViewWidget(Piece* _piece, QWidget* _parent) : QWidget(_parent)
{
	// setup draw widget
	setMinimumSize(200, 200);
	this->piece = _piece;
	this->zoom = this->defaultZoom = 12.0;

	isDraggingControlPoint = false;
	draggedControlPointIndex = 0;
}

void PieceCustomizeViewWidget :: resetZoom()
{
	this->zoom = this->defaultZoom;
	updateEverything();
}

QRect PieceCustomizeViewWidget :: usedRect()
{
	return QRect(ulX, ulY, squareSize, squareSize);
}

void PieceCustomizeViewWidget :: wheelEvent(QWheelEvent *e)
{
	if (e->delta() > 0)
		this->zoom *= 0.8;
	else if (e->delta() < 0)	
		this->zoom *= 1.2;
	updateEverything();
}

void PieceCustomizeViewWidget :: resizeEvent(QResizeEvent* event)
{
	int width, height;

	width = event->size().width();
	height = event->size().height();

	if (width > height) // wider than tall 
	{
		ulX = (width - height)/2.0;
		ulY = 0;
		squareSize = height; 
	}
	else  
	{
		ulX = 0;
		ulY = (height - width)/2.0;
		squareSize = width; 
	}
}

float PieceCustomizeViewWidget :: adjustedX(float rawX)
{
	float blowup = squareSize / zoom;
	return (rawX - ulX) / blowup;
}

float PieceCustomizeViewWidget :: adjustedY(float rawY)
{
	float blowup = squareSize / zoom;
	return (rawY - ulY) / blowup;
}

float PieceCustomizeViewWidget :: rawX(float adjustedX)
{
	float blowup = squareSize / zoom;
	return adjustedX * blowup + ulX;
}

float PieceCustomizeViewWidget :: rawY(float adjustedY)
{
	float blowup = squareSize / zoom;
	return adjustedY * blowup + ulY;
}

float PieceCustomizeViewWidget :: rawScale(float adjustedScale)
{
	float blowup = squareSize / zoom;
	return adjustedScale * blowup; 
}

float PieceCustomizeViewWidget :: adjustedScale(float rawScale)
{
	float blowup = squareSize / zoom;
	return rawScale / blowup; 
}

Point2D PieceCustomizeViewWidget :: controlPointRawLocation(unsigned int index)
{
	Point2D cp = piece->spline.controlPoints()[index];

	Point2D p;
	p.x = rawX(zoom/2 - cp.x);
	p.y = rawY(zoom/2 - cp.y);

	return p;
} 

void PieceCustomizeViewWidget :: mousePressEvent(QMouseEvent* event)
{
	// get press position
	Point2D mouse;
	mouse.x = event->pos().x();
	mouse.y = event->pos().y();
	
	// check and see if it's on a control point
	for (unsigned int i = 0; i < piece->spline.controlPoints().size(); ++i)
	{
		Point2D p = controlPointRawLocation(i);
		if (fabs(mouse.x - p.x) + fabs(mouse.y - p.y) < 4 * MAX(squareSize / 100, 1))
		{
			isDraggingControlPoint = true;
			draggedControlPointIndex = i;
			return;
		}
	}	
}

void PieceCustomizeViewWidget :: mouseMoveEvent(QMouseEvent* event)
{
	if (!isDraggingControlPoint)
		return;

	float center = zoom/2; 
	if (draggedControlPointIndex == 0)
	{
		// only adjust Y
		Point2D p = piece->spline.controlPoints()[0];
		p.y = center - adjustedY(event->pos().y());
		piece->spline.set(0, p);
	}
	else
	{
		Point2D p;
		p.x = center - adjustedX(event->pos().x());
		p.y = center - adjustedY(event->pos().y());
		piece->spline.set(draggedControlPointIndex, p);
	}
	emit someDataChanged();
}

void PieceCustomizeViewWidget :: mouseReleaseEvent(QMouseEvent*)
{
	isDraggingControlPoint = false;
}

void PieceCustomizeViewWidget :: updateEverything()
{
	this->repaint();	
}

void PieceCustomizeViewWidget :: setPiece(Piece* _piece)
{
	this->piece = _piece;
	resetZoom();
	updateEverything();
}

void PieceCustomizeViewWidget :: drawPiece()
{
	// draw the spline
	Spline& spline = piece->spline;

	// note that pixels are specified from upper left, so many Ys
	// are inverted. we assume a canvas of size 20 x 20 and adjust
	// using a "blowup" multiplier

	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);

	QPen pen;
	pen.setColor(QColor(0, 0, 0));
	pen.setWidth(MAX(squareSize/100, 1) * 2);
	pen.setCapStyle(Qt::RoundCap);
	painter.setPen(pen);
	painter.setBrush(QColor(100, 100, 100));

	// draw spline
	QPointF start;
	QPointF end;
	float center = zoom / 2.0;
	for (float t = 0.0; t < 1.0; t += 0.001)
	{
		// draw right side
		Point2D p = spline.get(t);
		start.setX(rawX(center + p.x));
		start.setY(rawY(center - p.y));	
		painter.drawPoint(start);

		// draw mirrored left side (y-values the same)
		start.setX(rawX(center - p.x));
		painter.drawPoint(start);
	}

	// draw control point connectors
        pen.setWidth(2);
        pen.setColor(Qt::white);
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);
	for (unsigned int i = 0; i < spline.controlPoints().size()-1; ++i)
	{
		Point2D p = controlPointRawLocation(i);
		start.setX(p.x);
		start.setY(p.y);
		p = controlPointRawLocation(i+1);
		end.setX(p.x);
		end.setY(p.y);
                painter.drawLine(start, end);
	}

	// draw control points
        painter.setBrush(QColor(0, 0, 0, 255));
        pen.setWidth(2);
        pen.setColor(Qt::white);
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);
	for (unsigned int i = 0; i < spline.controlPoints().size(); ++i)
	{
		Point2D p = controlPointRawLocation(i);
		start.setX(p.x);
		start.setY(p.y);
		int pointRadius = MAX(squareSize / 100, 1) * 2;
                painter.drawEllipse(start, pointRadius, pointRadius);
	}

	painter.end();
}

void PieceCustomizeViewWidget :: paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(event->rect(), QColor(200, 200, 200));
	painter.end();
	drawPiece();
}




