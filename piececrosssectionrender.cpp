
#include <QPainter>
#include <QColor>
#include "piece.h"
#include "piececrosssectionrender.h"
#include "constants.h"

namespace PieceCrossSectionRender
{

void render(QPainter* painter, int targetSize, Piece* piece)
{
	painter->setRenderHint(QPainter::Antialiasing);
	Spline spline = piece->spline();

	// Drawing renders a viewSize x viewSize box centered at (0, 0)
	// and scale to the input pixel dimension (int size).
	float zoom = 12.0;

	// set up style
	QPen pen;
	pen.setColor(QColor(0, 0, 0));
	pen.setWidth(4);
	pen.setCapStyle(Qt::RoundCap);
	painter->setPen(pen);

	// draw 
	float center = zoom / 2.0;
	float blowup = targetSize / static_cast<float>(zoom);
	Point2D p;
	for (float t = 0.0; t < 1.0; t += 0.001)
	{
		p = spline.get(t);
		painter->drawPoint((center + p.x) * blowup, (center - p.y) * blowup);
		painter->drawPoint((center - p.x) * blowup, (center - p.y) * blowup);
	}

	// connect spline ends to give "volume" to the drawing
	p = spline.controlPoints().back();
	painter->drawLine((center + p.x) * blowup, (center - p.y) * blowup, 
		(center - p.x) * blowup, (center - p.y) * blowup);
}

}


