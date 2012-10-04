
#ifndef SUBPICKUPTEMPLATE_H
#define SUBPICKUPTEMPLATE_H

#include "primitives.h"
#include "pullplan.h"
#include "shape.h"

// Pickup cane orientations
enum PickupCaneOrientation
{
        HORIZONTAL_PICKUP_CANE_ORIENTATION,
        VERTICAL_PICKUP_CANE_ORIENTATION,
        MURRINE_PICKUP_CANE_ORIENTATION
};

class SubpickupTemplate
{
	public:
		SubpickupTemplate(PullPlan* plan, Point location, int orientation, 
			float length, float width, enum GeometricShape shape);
		PullPlan* plan;
		Point location;
		int orientation;
		float length;
		float width;
		enum GeometricShape shape;
};


#endif

