

#include "subpulltemplate.h"

SubpullTemplate :: SubpullTemplate(PullPlan* plan, enum GeometricShape _s, Point location, float diameter)
{
	this->plan = plan;
	this->shape = _s;
	this->location = location;
	this->diameter = diameter;
}

void SubpullTemplate :: rescale(float ratio) 
{
	this->location.x *= ratio;
	this->location.y *= ratio;
	this->diameter *= ratio;
}


