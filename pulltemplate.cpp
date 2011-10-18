
#include "pulltemplate.h"

PullTemplate :: PullTemplate(int t)
{
	Point p;

	this->type = t;

	switch (t)
	{
		case CIRCLE_BASE_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			break;
		case SQUARE_BASE_TEMPLATE:
			this->shape = SQUARE_SHAPE;
			break;
		case AMORPHOUS_BASE_TEMPLATE:
			this->shape = AMORPHOUS_SHAPE;
			break;
		case SQUARE_FOUR_SQUARES_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			this->shape = SQUARE_SHAPE;
			for (int i = 0; i < 2; ++i)
			{
				for (int j = 0; j < 2; ++j)
				{
					p.x = -0.3 + 0.6 * i;
					p.y = -0.3 + 0.6 * j;
					subpulls.push_back(SubpullTemplate(SQUARE_SHAPE, p, 0.5, 0));
				}
			}
			break;
		case LINE_THREE_CIRCLES_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			this->shape = CIRCLE_SHAPE;
			for (int i = 0; i < 3; ++i)
			{
				p.x = -0.6666666 + 0.6666 * i;
				subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.60, 0));
			}
			break;	
		case LINE_FIVE_CIRCLES_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			this->shape = CIRCLE_SHAPE;
			for (int i = 0; i < 5; ++i)
			{
				p.x = -0.8 + 0.4 * i;
				subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.39, 0));
			}
			break;			
		case SQUARE_FOUR_CIRCLES_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			this->shape = CIRCLE_SHAPE;
			for (int i = 0; i < 2; ++i)
			{
				for (int j = 0; j < 2; ++j)
				{
					p.x = -0.3 + 0.6 * i;
					p.y = -0.3 + 0.6 * j;
					subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.59, 0));
				}
			}
			break;			
		case X_NINE_CIRCLES_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			this->shape = CIRCLE_SHAPE;
			for (int i = 0; i < 5; ++i)
			{
				p.x = -0.8 + 0.4 * i;
				p.y = 0.0;
				subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.39, 0));
			}
			for (int i = 0; i < 5; ++i)
			{
				if (i == 2)
					continue;
				p.x = 0.0;
				p.y = -0.8 + 0.4 * i;
				subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.39, 0));
			}
			break;	
	}
}




