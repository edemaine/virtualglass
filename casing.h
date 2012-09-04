
#ifndef CASING_H
#define CASING_H

#include "primitives.h"
#include "constants.h"
#include "glasscolor.h"

class Casing
{
	public:
		Casing(float thickness, int shape, GlassColor* glassColor);
		float thickness;	
		int shape;
		GlassColor* glassColor;
};

#endif

