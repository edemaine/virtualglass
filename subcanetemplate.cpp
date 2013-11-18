

#include "subcanetemplate.h"

SubcaneTemplate :: SubcaneTemplate(Cane* cane, enum GeometricShape _s, Point2D location, float diameter)
{
	this->cane = cane;
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


