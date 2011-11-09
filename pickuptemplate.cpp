
#include "pickuptemplate.h"

PickupTemplate :: PickupTemplate(int t)
{
	Point p;

	this->type = t;

	switch (t)
	{
		case TEN_VERTICALS_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			for (int i = 0; i < 10; ++i)
			{
				p.x = -0.9 + 0.2 * i;
				p.y = -0.99;
				subpulls.push_back(SubpickupTemplate(p, VERTICAL_ORIENTATION, 1.99, 0.19, 0));
			}
			break;
		case FIFTEEN_VERTICALS_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			for (int i = 0; i < 15; ++i)
			{
				p.x = -0.9 + 0.1333 * i;
				p.y = -0.99;
				subpulls.push_back(SubpickupTemplate(p, VERTICAL_ORIENTATION, 1.99, 0.13, 0));
			}
			break;
		case TWENTY_VERTICALS_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			for (int i = 0; i < 20; ++i)
			{
				p.x = -0.9 + 0.1 * i;
				p.y = -0.99;
				subpulls.push_back(SubpickupTemplate(p, VERTICAL_ORIENTATION, 1.99, 0.09, 0));
			}
			break;
		case FOUR_SQUARES_OF_TEN_VERTICALS_TEMPLATE:
			for (int i = 0; i < 10; ++i)
			{
				p.x = -0.95 + 0.1 * i;
				p.y = 0.01;
				subpulls.push_back(SubpickupTemplate(p, VERTICAL_ORIENTATION, 0.99, 0.09, 0));
			}
			for (int i = 0; i < 10; ++i)
			{
				p.x = -0.95 + 0.1 * i;
				p.y = -0.99;
				subpulls.push_back(SubpickupTemplate(p, VERTICAL_ORIENTATION, 0.99, 0.09, 0));
			}
			for (int i = 0; i < 10; ++i)
			{
				p.x = 0.05 + 0.1 * i;
				p.y = -0.99;
				subpulls.push_back(SubpickupTemplate(p, VERTICAL_ORIENTATION, 0.99, 0.09, 0));
			}
			for (int i = 0; i < 10; ++i)
			{
				p.x = 0.05 + 0.1 * i;
				p.y = 0.01;
				subpulls.push_back(SubpickupTemplate(p, VERTICAL_ORIENTATION, 0.99, 0.09, 0));
			}
			break;
	}
}




