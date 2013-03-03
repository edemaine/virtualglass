
#include <QPainter>
#include <QColor>
#include "pullplan.h"
#include "pullplancrosssectionrender.h"
#include "glasscolor.h"
#include "globalbackgroundcolor.h"

namespace PullPlanCrossSectionRender
{

void setBoundaryPainter(QPainter* painter, bool outermostLevel) 
{

	if (outermostLevel)
	{
		QPen pen;
		pen.setWidth(2);
		pen.setColor(Qt::black);
		painter->setPen(pen);
	}
	else 
	{
		painter->setPen(Qt::NoPen);
	}
}

void paintShape(float x, float y, float size, enum GeometricShape shape, QPainter* painter)
{
	int roundedX, roundedY;

	roundedX = floor(x + 0.5);
	roundedY = floor(y + 0.5);

	switch (shape)
	{
		case CIRCLE_SHAPE:
			painter->drawEllipse(roundedX, roundedY, size, size);
			break;
		case SQUARE_SHAPE:
			painter->drawRect(roundedX, roundedY, size, size);
			break;
	}
	
}


void drawSubplan(float x, float y, float drawWidth, float drawHeight, 
	PullPlan* plan, bool outermostLevel, QPainter* painter) 
{
	painter->setBrush(GlobalBackgroundColor::qcolor);
	painter->setPen(Qt::NoPen);
	paintShape(x, y, drawWidth, plan->getOutermostCasingShape(), painter);

	// Do casing colors outermost to innermost to get concentric rings of each casing's color
	for (unsigned int i = plan->getCasingCount() - 1; i < plan->getCasingCount(); --i) 
	{
		int casingWidth = drawWidth * plan->getCasingThickness(i);
		int casingHeight = drawHeight * plan->getCasingThickness(i);
		int casingX = x + drawWidth / 2 - casingWidth / 2;
		int casingY = y + drawHeight / 2 - casingHeight / 2;

		// Fill with solid neutral grey (in case fill is transparent)
		painter->setBrush(GlobalBackgroundColor::qcolor);
		painter->setPen(Qt::NoPen); // Will draw boundary after all filling is done
		paintShape(casingX, casingY, casingWidth, plan->getCasingShape(i), painter);
		
		// Fill with actual casing color (highlighting white or some other color)
		Color c = plan->getCasingColor(i)->getColor();
		QColor qc(255*c.r, 255*c.g, 255*c.b, 255*c.a);
		painter->setBrush(qc);

		setBoundaryPainter(painter, outermostLevel);
		paintShape(casingX, casingY, casingWidth, plan->getCasingShape(i), painter);
	}

	// Recursively call drawing on subplans
	for (unsigned int i = plan->subs.size()-1; i < plan->subs.size(); --i)
	{
		SubpullTemplate* sub = &(plan->subs[i]);

		float rX = x + (sub->location.x - sub->diameter/2.0) * drawWidth/2 + drawWidth/2;
		float rY = y + (sub->location.y - sub->diameter/2.0) * drawWidth/2 + drawHeight/2;
		float rWidth = sub->diameter * drawWidth/2;
		float rHeight = sub->diameter * drawHeight/2;

		drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan, 
			false, painter);

		painter->setBrush(Qt::NoBrush);
		setBoundaryPainter(painter, outermostLevel); 
		paintShape(rX, rY, rWidth, plan->subs[i].shape, painter);
	}
}

void render(QPainter* painter, int size, PullPlan* plan)
{
	painter->setRenderHint(QPainter::Antialiasing);

	drawSubplan(5, 5, size - 10, size - 10, plan, true, painter);
	painter->setBrush(Qt::NoBrush);
	setBoundaryPainter(painter, true);
	paintShape(5, 5, size - 10, plan->getOutermostCasingShape(), painter);
}

} // end namespace

