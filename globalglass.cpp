
#include "globalglass.h"
#include "cane.h"
#include "glasscolor.h"
#include "pulltemplate.h"
#include "shape.h"

namespace GlobalGlass
{
	GlassColor* _color = 0;
	Cane* _circleCane = 0;
	Cane* _squareCane = 0;

	GlassColor* color()
	{
		if (_color == 0)
			_color = new GlassColor();
		return _color;
	}
	
	Cane* circleCane()
	{
		if (_circleCane == 0)
			_circleCane = new Cane(PullTemplate::BASE_CIRCLE);
		return _circleCane;
	}

	Cane* squareCane()
	{
		if (_squareCane == 0)
			_squareCane = new Cane(PullTemplate::BASE_SQUARE);
		return _squareCane;
	}
}

