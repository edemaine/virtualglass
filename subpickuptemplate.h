
#ifndef SUBPICKUPTEMPLATE_H
#define SUBPICKUPTEMPLATE_H

#include "primitives.h"
#include "pullplan.h"

class SubpickupTemplate
{
	public:
		SubpickupTemplate(PullPlan* plan, Point location, int orientation, 
			float length, float width, int shape, int group);
		PullPlan* plan;
		Point location;
		int orientation;
		float length;
		float width;
		int shape;
		int group;
};


#endif

