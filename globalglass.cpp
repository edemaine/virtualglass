
#include "globalglass.h"
#include "pullplan.h"
#include "glasscolor.h"
#include "pulltemplate.h"
#include "shape.h"

namespace GlobalGlass
{
        GlassColor* _color = 0;
        PullPlan* _circlePlan = 0;
        PullPlan* _squarePlan = 0;

        GlassColor* color()
	{
		if (_color == 0)
			_color = new GlassColor();
		return _color;
	}
	
        PullPlan* circlePlan()
	{
		if (_circlePlan == 0)
			_circlePlan = new PullPlan(PullTemplate::BASE_CIRCLE);
		return _circlePlan;
	}

        PullPlan* squarePlan()
	{
		if (_squarePlan == 0)
		{
			_squarePlan = new PullPlan(PullTemplate::BASE_CIRCLE);
			_squarePlan->setOutermostCasingShape(SQUARE_SHAPE);
		}
		return _squarePlan;
	}
}

