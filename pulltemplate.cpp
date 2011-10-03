
#include "pulltemplate.h"

PullTemplate :: PullTemplate()
{
	clearTemplate();
}

void PullTemplate :: clearTemplate()
{
	locations.clear();
	diameters.clear();
}

void PullTemplate :: addSubcane(Point location, float diameter)
{
	locations.push_back(location);
	diameters.push_back(diameter);
} 


