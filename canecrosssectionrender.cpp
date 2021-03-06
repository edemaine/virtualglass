
#include <QPainter>
#include <QColor>
#include "cane.h"
#include "canecrosssectionrender.h"
#include "glasscolor.h"
#include "globalbackgroundcolor.h"

// TODO: Replace similar code in CaneEditorViewWidget and CaneCustomizeViewWidget with calls to this code
// (Will require some small changes to allow the slightly different rendering of those versions)
namespace CaneCrossSectionRender
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
	int roundedX = floor(x + 0.5);
	int roundedY = floor(y + 0.5);
	int roundedSize = size;
	if (roundedSize < 1) // If size is 0, drawEllipse/drawRect will cause a failed assertion it seems
		return;	

	switch (shape)
	{
		case CIRCLE_SHAPE:
			painter->drawEllipse(roundedX, roundedY, roundedSize, roundedSize);
			break;
		case SQUARE_SHAPE:
			painter->drawRect(roundedX, roundedY, roundedSize, roundedSize);
			break;
	}
}


void drawSubcane(float x, float y, float drawWidth, float drawHeight, 
	Cane* cane, bool outermostLevel, QPainter* painter) 
{
	painter->setBrush(GlobalBackgroundColor::qcolor);
	painter->setPen(Qt::NoPen);
	paintShape(x, y, drawWidth, cane->outermostCasingShape(), painter);

	// Do casing colors outermost to innermost to get concentric rings of each casing's color
	for (unsigned int i = cane->casingCount() - 1; i < cane->casingCount(); --i) 
	{
		int casingWidth = drawWidth * cane->casingThickness(i);
		int casingHeight = drawHeight * cane->casingThickness(i);
		int casingX = x + drawWidth / 2 - casingWidth / 2;
		int casingY = y + drawHeight / 2 - casingHeight / 2;

		// Fill with solid neutral grey (in case fill is transparent)
		painter->setBrush(GlobalBackgroundColor::qcolor);
		painter->setPen(Qt::NoPen); // Will draw boundary after all filling is done
		paintShape(casingX, casingY, casingWidth, cane->casingShape(i), painter);
		
		// Fill with actual casing color (highlighting white or some other color)
		Color c = cane->casingColor(i)->color();
		QColor qc;
		qc.setRgbF(c.r, c.g, c.b, c.a);
		painter->setBrush(qc);

		setBoundaryPainter(painter, outermostLevel);
		paintShape(casingX, casingY, casingWidth, cane->casingShape(i), painter);
	}

	// Recursively call drawing on subcanes
	for (unsigned int i = cane->subcaneCount()-1; i < cane->subcaneCount(); --i)
	{
		SubcaneTemplate sub = cane->subcaneTemplate(i);

		float rX = x + (sub.location.x - sub.diameter/2.0) * drawWidth/2 + drawWidth/2;
		float rY = y + (sub.location.y - sub.diameter/2.0) * drawWidth/2 + drawHeight/2;
		float rWidth = sub.diameter * drawWidth/2;
		float rHeight = sub.diameter * drawHeight/2;

		drawSubcane(rX, rY, rWidth, rHeight, sub.cane, 
			false, painter);

		painter->setBrush(Qt::NoBrush);
		setBoundaryPainter(painter, outermostLevel); 
		paintShape(rX, rY, rWidth, sub.shape, painter);
	}
}

void render(QPainter* painter, int size, Cane* cane)
{
	painter->setRenderHint(QPainter::Antialiasing);

	drawSubcane(5, 5, size - 10, size - 10, cane, true, painter);
	painter->setBrush(Qt::NoBrush);
	setBoundaryPainter(painter, true);
	paintShape(5, 5, size - 10, cane->outermostCasingShape(), painter);
}

} // end namespace

