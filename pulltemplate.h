
#ifndef PULLTEMPLATE_H
#define PULLTEMPLATE_H

#include <vector>
#include "primitives.h"

using std::vector;

class PullTemplate
{
	public:
		PullTemplate();
		void clearTemplate();
		void addSubcane(Point location, float diameter);
		vector<Point> locations;
		vector<float> diameters; 
};

#endif

