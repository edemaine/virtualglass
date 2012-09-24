

#include "subpickuptemplate.h"

SubpickupTemplate :: SubpickupTemplate(PullPlan* _plan, Point _location, int _orientation, float _length, 
	float _width, enum GeometricShape _shape, int _group)
{
	plan = _plan;
	location = _location;
	orientation = _orientation;
	length = _length;
	width = _width;
	shape = _shape;
	group = _group;
}


