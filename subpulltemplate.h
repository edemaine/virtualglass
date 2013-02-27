
#ifndef SUBPULLTEMPLATE_H
#define SUBPULLTEMPLATE_H

#include "primitives.h"
#include "shape.h"

class PullPlan;

class SubpullTemplate
{
	public:
		SubpullTemplate(PullPlan* plan, enum GeometricShape s, Point2D location, float diameter);
		PullPlan* plan;
		enum GeometricShape shape;
		Point2D location;
		float diameter;

		void rescale(float ratio);
};


#endif

