
#ifndef SUBPICKUPTEMPLATE_H
#define SUBPICKUPTEMPLATE_H

#include "primitives.h"

class SubpickupTemplate
{
	public:
		SubpickupTemplate(Point location, int orientation, float length, float width);
		Point location;
		int orientation;
		float length;
		float width;
};


#endif
