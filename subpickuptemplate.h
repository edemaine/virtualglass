
#ifndef SUBPICKUPTEMPLATE_H
#define SUBPICKUPTEMPLATE_H

#include "primitives.h"

class SubpickupTemplate
{
	public:
		SubpickupTemplate(Point location, int orientation, float length, float width, int shape, int group);
		Point location;
		int orientation;
		float length;
		float width;
		int shape;
		int group;
};


#endif

