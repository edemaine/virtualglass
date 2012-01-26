
#ifndef SUBPULLTEMPLATE_H
#define SUBPULLTEMPLATE_H

#include "primitives.h"
#include "pullplan.h"

class PullPlan;

class SubpullTemplate
{
	public:
		SubpullTemplate(PullPlan* plan, int shape, Point location, float diameter, int group);
		PullPlan* plan;
		int shape;
		Point location;
		float diameter;
		int group;
};


#endif

