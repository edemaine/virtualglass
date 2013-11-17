
#ifndef CANECROSSECTIONRENDER_H
#define CANECROSSECTIONRENDER_H

#include "primitives.h"
#include "shape.h"

class QPainter;
class PullPlan;

namespace CaneCrossSectionRender
{
	void paintShape(float x, float y, float size, enum GeometricShape s, QPainter* p);
	void setBoundaryPainter(QPainter* painter, bool outermostLevel);
	void drawSubplan(float x, float y, float width, float height, PullPlan* plan, 
		bool outermostLevel, QPainter* painter);
	void render(QPainter* painter, int size, PullPlan* plan);
}

#endif

