
#include "pickuptemplate.h"

PickupTemplate :: PickupTemplate(int t)
{
	Point p;

	this->type = t;

	switch (t)
	{
		case TEN_HORIZONTALS_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			for (int i = 0; i < 10; ++i)
			{
				p.y = -0.9 + 0.2 * i;
				subpulls.push_back(SubpickupTemplate(p, HORIZONTAL_ORIENTATION, 1.99, 0.19, 0));
			}
			break;
		case FIVE_AND_FIVE_HORIZONTALS_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			for (int i = 0; i < 10; ++i)
			{
				p.y = -0.9 + 0.2 * i;
				subpulls.push_back(SubpickupTemplate(p, HORIZONTAL_ORIENTATION, 1.99, 0.19, i % 2));
			}
			break;
		case TEN_VERTICALS_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			for (int i = 0; i < 10; ++i)
			{
				p.x = -0.9 + 0.2 * i;
				subpulls.push_back(SubpickupTemplate(p, VERTICAL_ORIENTATION, 1.99, 0.19, 0));
			}
			break;
		case FIVE_AND_FIVE_VERTICALS_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			for (int i = 0; i < 10; ++i)
			{
				p.x = -0.9 + 0.2 * i;
				subpulls.push_back(SubpickupTemplate(p, VERTICAL_ORIENTATION, 1.99, 0.19, i % 2));
			}
			break;
	}
}




