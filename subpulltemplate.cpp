

#include "subpulltemplate.h"

SubpullTemplate :: SubpullTemplate(PullPlan* plan, int shape, Point location, float diameter, int group)
{
	this->plan = plan;
	this->shape = shape;
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


