
#include "pickuptemplate.h"

PickupTemplate :: PickupTemplate(int t)
{
	Point p;

	this->type = t;

	switch (t)
	{
		case TWENTY_HORIZONTALS_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			for (int i = 0; i < 20; ++i)
			{
				p.x = -0.99;
				p.y = -0.9 + 0.1 * i;
				subpulls.push_back(SubpickupTemplate(p, HORIZONTAL_ORIENTATION, 1.99, 0.09, 0));
			}
			break;
		case TEN_AND_TEN_HORIZONTALS_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			for (int i = 0; i < 20; ++i)
			{
				p.x = -0.99;
				p.y = -0.9 + 0.1 * i;
				subpulls.push_back(SubpickupTemplate(p, HORIZONTAL_ORIENTATION, 1.99, 0.09, i % 2));
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
		case TEN_AND_TEN_VERTICALS_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			for (int i = 0; i < 20; ++i)
			{
				p.x = -0.9 + 0.1 * i;
				p.y = -0.99;
				subpulls.push_back(SubpickupTemplate(p, VERTICAL_ORIENTATION, 1.99, 0.09, i % 2));
			}
			break;
		case SIX_COLUMNS_OF_TWENTY_HORIZONTALS_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			for (int i = 0; i < 7; ++i)
			{
				p.x = -0.9 + 0.3 * i;
				p.y = -0.99;
				subpulls.push_back(SubpickupTemplate(p, VERTICAL_ORIENTATION, 1.99, 0.05, 0));
			}
			for (int i = 0; i < 6; ++i)
			{
				for (int j = 0; j < 20; ++j)
				{
					p.x = -0.85 + 0.3 * i;
					p.y = -0.95 + 0.1 * j;
					subpulls.push_back(SubpickupTemplate(p, HORIZONTAL_ORIENTATION, 0.19, 0.09, 1));
				}
			}
			break;
		case FOUR_SQUARES_OF_TEN_VERTICALS_TEMPLATE:
			for (int i = 0; i < 10; ++i)
			{
				p.x = -0.9 + 0.1 * i;
				p.y = 0.01;
				subpulls.push_back(SubpickupTemplate(p, VERTICAL_ORIENTATION, 0.99, 0.09, 0));
			}
			for (int i = 0; i < 10; ++i)
			{
				p.x = -0.9 + 0.1 * i;
				p.y = -0.99;
				subpulls.push_back(SubpickupTemplate(p, VERTICAL_ORIENTATION, 0.99, 0.09, 1));
			}
			for (int i = 0; i < 10; ++i)
			{
				p.x = 0.1 + 0.1 * i;
				p.y = -0.99;
				subpulls.push_back(SubpickupTemplate(p, VERTICAL_ORIENTATION, 0.99, 0.09, 2));
			}
			for (int i = 0; i < 10; ++i)
			{
				p.x = 0.1 + 0.1 * i;
				p.y = 0.01;
				subpulls.push_back(SubpickupTemplate(p, VERTICAL_ORIENTATION, 0.99, 0.09, 3));
			}
			break;
	}
}




