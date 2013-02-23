
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

	isDraggingPoint = false;
	draggedPointIndex = 0;
}

QRect PieceCustomizeViewWidget :: usedRect()
{
	return QRect(ulX, ulY, squareSize, squareSize);
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
	return rawX - ulX;
}

float PieceCustomizeViewWidget :: adjustedY(float rawY)
{
	return rawY - ulY;
}

float PieceCustomizeViewWidget :: rawX(float adjustedX)
{
	return adjustedX + ulX;
}

float PieceCustomizeViewWidget :: rawY(float adjustedY)
{
	return adjustedY + ulY;
}

unsigned int PieceCustomizeViewWidget :: choose(unsigned int n, unsigned int k)
{
	if (n < k)
		return 0;
	unsigned int total = 1;
	for (unsigned int i = n; i > n - k; --i)
		total *= i; // get the numerator
	for (unsigned int i = k; i > 0; --i)
		total /= i; // get the denominator
	return total;
}

float PieceCustomizeViewWidget :: splineVal(vector<float>& spline, float t)
{
	float val = 0;
	for (unsigned int i = 0; i < spline.size(); ++i)
		val += choose(spline.size()-1, i) * pow((1.0 - t), spline.size() - 1 - i) * pow(t, i) * spline[i]; 
	return val;
}

void PieceCustomizeViewWidget :: mousePressEvent(QMouseEvent* /*event*/)
{

#ifdef UNDEF
	float x = (adjustedX(event->pos().x()) - squareSize/2) / float(squareSize/2-10);
	float y = (adjustedY(event->pos().y()) - squareSize/2) / float(squareSize/2-10);

	// Check for casing resize
	for (unsigned int i = 0; i < plan->getCasingCount() - 1; ++i) 
	{
		if (isOnCasing(i, x, y))
		{
			isDraggingCasing = true; 
			draggedCasingIndex = i;
			return;
		}	
	}

	// Check for convenience subplan-to-subplan drag
	PullPlan* selectedSubplan = getSubplanAt(x, y);
	if (selectedSubplan != NULL)
	{
		enum GlassMime::Type type = GlassMime::PULLPLAN_MIME;
		AsyncPullPlanLibraryWidget plplw(selectedSubplan);		
		QPixmap pixmap = *(plplw.getDragPixmap());

	        char buf[500];
		GlassMime::encode(buf, selectedSubplan, type);
		QByteArray pointerData(buf);
		QMimeData* mimeData = new QMimeData;
		mimeData->setText(pointerData);

		QDrag *drag = new QDrag(this);
		drag->setMimeData(mimeData);
		drag->setPixmap(pixmap);

		drag->exec(Qt::CopyAction);
	}	
#endif
}

void PieceCustomizeViewWidget :: mouseMoveEvent(QMouseEvent* /*event*/)
{
#ifdef UNDEF
	if (!isDraggingCasing)
		return;

	float x = (adjustedX(event->pos().x()) - squareSize/2);
	float y = (adjustedY(event->pos().y()) - squareSize/2);
	float radius = getShapeRadius(plan->getCasingShape(draggedCasingIndex), x, y) / (squareSize/2 - 10);

	float min;
	float max;

	setMinMaxCasingRadii(&min, &max);	
	plan->setCasingThickness(MIN(MAX(radius, min), max), draggedCasingIndex);

	emit someDataChanged();
#endif
}

void PieceCustomizeViewWidget :: mouseReleaseEvent(QMouseEvent* /*event*/)
{
#ifdef UNDEF
	isDraggingCasing = false;
#endif
}

void PieceCustomizeViewWidget :: updateEverything()
{
	this->repaint();	
}

void PieceCustomizeViewWidget :: setPiece(Piece* _piece)
{
	this->piece = _piece;
	updateEverything();
}

void PieceCustomizeViewWidget :: drawPiece()
{
	// draw the spline
	vector<float>& spline = piece->spline;

	// note that pixels are specified from upper left, so many Ys
	// are inverted. we assume a canvas of size 20 x 20 and adjust
	// using a "blowup" multiplier

	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);

	QPen pen;
	pen.setColor(QColor(0, 0, 0));
	pen.setWidth(7);
	pen.setCapStyle(Qt::RoundCap);
	painter.setPen(pen);
	painter.setBrush(QColor(100, 100, 100));

	// total line length is 10, with spline[0] determining bottom width,
	// and spline determining remaining side curve, after a PI/2 turn with 
	// length 1.0
	float size = 25.0;
	float blowup = squareSize / size;
	
	// first draw bottom
	QPointF start;
	start.setX((size * 0.5 - spline[0]) * blowup);
	start.setY(size * 0.75 * blowup);

	QPointF end;
	end.setX((size * 0.5 + spline[0]) * blowup);
	end.setY(size * 0.75 * blowup);
	
	painter.drawLine(QLineF(start, end));

	// next draw turn
	QPointF center;
	float turnCenterX = size * 0.5 + spline[0];
	float turnCenterY = size * 0.75 - 4 / PI;
	painter.drawArc((turnCenterX - 4 / PI) * blowup, (turnCenterY - 4 / PI) * blowup, 8 / PI * blowup, 8 / PI * blowup,
		0 * 16, -90 * 16); 
	turnCenterX = size * 0.5 - spline[0];
	turnCenterY = size * 0.75 - 4 / PI;
	painter.drawArc((turnCenterX - 4 / PI) * blowup, (turnCenterY - 4 / PI) * blowup, 8 / PI * blowup, 8 / PI * blowup,
		-180 * 16, 90 * 16); 
	
	// now draw remainder	
	for (float t = 0.0; t < 1.0; t += 0.01)
	{
		float t_delta = t + 0.01;	

		start.setX((size * 0.5 + splineVal(spline, t) + 4/PI) * blowup);
		start.setY((size * 0.75 - (4/PI + t * (9.0 - spline[0]))) * blowup);	
		end.setX((size * 0.5 + splineVal(spline, t_delta) + 4/PI) * blowup);
		end.setY((size * 0.75 - (4/PI + t_delta * (9.0 - spline[0]))) * blowup);	
		painter.drawLine(QLineF(start, end));

		start.setX((size * 0.5 - splineVal(spline, t) - 4/PI) * blowup);
		end.setX((size * 0.5 - splineVal(spline, t_delta) - 4/PI) * blowup);
		painter.drawLine(QLineF(start, end));
	}

	// finally, draw control points
	

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
