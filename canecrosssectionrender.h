
#ifndef CANECROSSECTIONRENDER_H
#define CANECROSSECTIONRENDER_H

#include "primitives.h"
#include "shape.h"

class QPainter;
class Cane;

namespace CaneCrossSectionRender
{
	void paintShape(float x, float y, float size, enum GeometricShape s, QPainter* p);
	void setBoundaryPainter(QPainter* painter, bool outermostLevel);
	void drawSubcane(float x, float y, float width, float height, Cane* cane, 
		bool outermostLevel, QPainter* painter);
	void render(QPainter* painter, int size, Cane* cane);
}

#endif

