

#include "subcanetemplate.h"

SubcaneTemplate :: SubcaneTemplate(Cane* plan, enum GeometricShape _s, Point2D location, float diameter)
{
	this->plan = plan;
	this->shape = _s;
	this->location = location;
	this->diameter = diameter;
}

void SubcaneTemplate :: rescale(float ratio) 
{
	this->location.x *= ratio;
	this->location.y *= ratio;
	this->diameter *= ratio;
}


