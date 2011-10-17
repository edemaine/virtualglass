
#ifndef SUBPULLTEMPLATE_H
#define SUBPULLTEMPLATE_H

#include "primitives.h"

class SubpullTemplate
{
	public:
		SubpullTemplate(int shape, Point location, float diameter, int group);
		int shape;
		Point location;
		float diameter;
		int group;
};


#endif

