
#include "pickuptemplate.h"

PickupTemplate :: PickupTemplate(int t)
{
	Point p;

	this->type = t;

	switch (t)
	{
		case THREE_HORIZONTALS_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			for (int i = 0; i < 3; ++i)
			{
				p.y = -0.6666666 + 0.6666 * i;
				subpulls.push_back(SubpickupTemplate(p, HORIZONTAL_ORIENTATION, 1.9, 0.5));
			}
			break;
		case THREE_VERTICALS_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			for (int i = 0; i < 3; ++i)
			{
				p.x = -0.6666666 + 0.6666 * i;
				subpulls.push_back(SubpickupTemplate(p, VERTICAL_ORIENTATION, 1.9, 0.5));
			}
			break;
	}
}




