
#include "globalglass.h"
#include "cane.h"
#include "glasscolor.h"
#include "pulltemplate.h"
#include "shape.h"

namespace GlobalGlass
{
	GlassColor* _color = 0;
	Cane* _circlePlan = 0;
	Cane* _squarePlan = 0;

	GlassColor* color()
	{
		if (_color == 0)
			_color = new GlassColor();
		return _color;
	}
	
	Cane* circlePlan()
	{
		if (_circlePlan == 0)
			_circlePlan = new Cane(PullTemplate::BASE_CIRCLE);
		return _circlePlan;
	}

	Cane* squarePlan()
	{
		if (_squarePlan == 0)
			_squarePlan = new Cane(PullTemplate::BASE_SQUARE);
		return _squarePlan;
	}
}

