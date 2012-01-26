

#include "subpickuptemplate.h"

SubpickupTemplate :: SubpickupTemplate(PullPlan* plan, Point location, int orientation, float length, float width, int shape, int group)
{
	this->plan = plan;
	this->location = location;
	this->orientation = orientation;
	this->length = length;
	this->width = width;
	this->shape = shape;
	this->group = group;
}


