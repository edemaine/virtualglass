
#ifndef SUBPICKUPTEMPLATE_H
#define SUBPICKUPTEMPLATE_H

#include "primitives.h"
#include "shape.h"

class Cane;

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
		SubpickupTemplate(Cane* cane, Point3D location, enum PickupCaneOrientation orientation, 
			float length, float width, enum GeometricShape shape);
		Cane* cane;
		Point3D location;
		enum PickupCaneOrientation orientation;
		float length;
		float width;
		enum GeometricShape shape;
};


#endif

