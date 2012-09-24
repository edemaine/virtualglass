
#ifndef CASING_H
#define CASING_H

#include "primitives.h"
#include "shape.h"
#include "glasscolor.h"

class Casing
{
	public:
		Casing(float thickness, enum GeometricShape s, GlassColor* gc);
		float thickness;	
		enum GeometricShape shape;
		GlassColor* glassColor;
};

#endif

