
#ifndef GLOBALGLASS_H
#define GLOBALGLASS_H 

class GlassColor;
class Cane;

namespace GlobalGlass
{
	// call these, as they're guaranteed to get you something valid.
	// there is risk for infinite recursion, as calling circlePlan()
	// or squarePlan can trigger the creation of a new pull plan, 
	// which might call these, etc. This is avoided because the pull plans
	// created by circlePlan() and squarePlan() calls have no subplans, 
	// so *they do not call circlePlan() and squarePlan()*.
	// Hopefully this doesn't change o_o
	GlassColor* color();
	Cane* circlePlan();
	Cane* squarePlan();

}

#endif

