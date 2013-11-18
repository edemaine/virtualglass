
#ifndef SUBCANETEMPLATE_H
#define SUBCANETEMPLATE_H

#include "primitives.h"
#include "shape.h"

class Cane;

class SubcaneTemplate
{
	public:
		SubcaneTemplate(Cane* cane, enum GeometricShape s, Point2D location, float diameter);
		Cane* cane;
		enum GeometricShape shape;
		Point2D location;
		float diameter;

		void rescale(float ratio);
};


#endif

