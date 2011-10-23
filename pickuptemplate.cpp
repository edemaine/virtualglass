
#include "pickuptemplate.h"

PickupTemplate :: PickupTemplate(int t)
{
	Point p;

	this->type = t;

	switch (t)
	{
		case THREE_HORIZONTALS_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			for (int i = 0; i < 10; ++i)
			{
				p.y = -0.9 + 0.2 * i;
				subpulls.push_back(SubpickupTemplate(p, HORIZONTAL_ORIENTATION, 1.9, 0.1, 0));
			}
			break;
		case THREE_VERTICALS_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			for (int i = 0; i < 10; ++i)
			{
				p.x = -0.9 + 0.2 * i;
				subpulls.push_back(SubpickupTemplate(p, VERTICAL_ORIENTATION, 1.9, 0.1, 0));
			}
			break;
	}
}




