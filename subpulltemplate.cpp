

#include "subpulltemplate.h"

SubpullTemplate :: SubpullTemplate(PullPlan* plan, enum GeometricShape _s, Point location, float diameter, int group)
{
	this->plan = plan;
	this->shape = _s;
	this->location = location;
	this->diameter = diameter;
	this->group = group;
}

void SubpullTemplate :: rescale(float ratio) 
{
	this->location.x *= ratio;
	this->location.y *= ratio;
	this->diameter *= ratio;
}


