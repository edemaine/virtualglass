
#ifndef SUBPULLTEMPLATE_H
#define SUBPULLTEMPLATE_H

#include "primitives.h"
#include "shape.h"

class PullPlan;

class SubpullTemplate
{
	public:
		SubpullTemplate(PullPlan* plan, enum GeometricShape s, Point location, float diameter);
		PullPlan* plan;
		enum GeometricShape shape;
		Point location;
		float diameter;

		void rescale(float ratio);
};


#endif

