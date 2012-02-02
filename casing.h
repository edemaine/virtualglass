
#ifndef CASING_H
#define CASING_H

#include "primitives.h"
#include "constants.h"

class Casing
{
	public:
		Casing();
		Casing(float thickness, int shape, Color* color);
		float thickness;	
		int shape;
		Color* color;
};

#endif

