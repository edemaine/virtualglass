

#include "subpickuptemplate.h"

SubpickupTemplate :: SubpickupTemplate(Cane* _cane, Point3D _location, PickupCaneOrientation _orientation, float _length, 
	float _width, enum GeometricShape _shape)
{
	cane = _cane;
	location = _location;
	orientation = _orientation;
	length = _length;
	width = _width;
	shape = _shape;
}


