
#ifndef GLOBALGLASS_H
#define GLOBALGLASS_H 

class GlassColor;
class Cane;

namespace GlobalGlass
{
	// call these, as they're guaranteed to get you something valid.
	// there is risk for infinite recursion, as calling circleCane()
	// or squareCane can trigger the creation of a new cane, 
	// which might call these, etc. This is avoided because the pull plans
	// created by circleCane() and squareCane() calls have no subplans, 
	// so *they do not call circleCane() and squareCane()*.
	// Hopefully this doesn't change o_o
	GlassColor* color();
	Cane* circleCane();
	Cane* squareCane();

}

#endif

