#include <math.h>
#include "pulltemplate.h"
#define sqrt2 1.41421356
#define sqrt3 1.73205081
#define pi 3.1415926535

PullTemplate :: PullTemplate(int type, float casingThickness)
{
	this->type = type;
	this->casingThickness = casingThickness;

	initializeSubpulls(); // need to just allocate the right number of subpulls with casing thickness 0
	updateSubpulls(); // need to change locations of subpulls, but nothing else
}

void PullTemplate :: updateSubpulls()
{
	Point p;
	float radius = 1.0 - casingThickness;

	p.x = p.y = p.z = 0.0;
	switch (this->type)
	{
		case CASED_CIRCLE_TEMPLATE:
			subpulls[0].diameter = radius * 1.90; 
			break;	
		case LINE_THREE_TEMPLATE:
			for (int i = 0; i < 3; ++i)
			{
				p.x = radius * (-0.6666 + 0.6666 * i);
				subpulls[i].location = p;
				subpulls[i].diameter = radius * 0.65; 
			}
			break;	
		case LINE_FIVE_TEMPLATE:
			for (int i = 0; i < 5; ++i)
			{
				p.x = radius * (-0.8 + 0.4 * i);
				subpulls[i].location = p;
				subpulls[i].diameter = radius * 0.39; 
			}
			break;			
		case CIRCLE_FOUR_TEMPLATE:
			for (int i = 0; i < 2; ++i)
			{
				for (int j = 0; j < 2; ++j)
				{
					p.x = radius * (-0.5 + 1 * i) * 0.8;
					p.y = radius * (-0.5 + 1 * j) * 0.8;
					subpulls[2*i + j].location = p;
					subpulls[2*i + j].diameter = radius * 1 * 0.8; 
				}
			}
			break;			
		case CIRCLE_SIX_TEMPLATE:
			for (int i = 0; i < 6; ++i)
			{
				p.x = radius * cos (2*pi*i/6) * 0.666;
				p.y = radius * sin (2*pi*i/6) * 0.666;
				subpulls[i].location = p;
				subpulls[i].diameter = radius * 1 * 0.666;
			}
			p.x = 0;
			p.y = 0;
			subpulls[6].location = p;
			subpulls[6].diameter = radius * 1 * 0.666;
			break;			
		case CIRCLE_TWELVE_TEMPLATE:
			for (int i = 0; i < 12; ++i)
			{
				p.x = radius * cos (2*pi*i/12) * 0.79;
				p.y = radius * sin (2*pi*i/12) * 0.79;
				subpulls[i].location = p;
				subpulls[i].diameter = radius * 1/2 * 0.79;
			}
			p.x = 0;
			p.y = 0;
			subpulls[12].location = p;
			subpulls[12].diameter = radius * 1*1.5 * 0.79;
			break;			
		case SQUARE_FOUR_TEMPLATE:
			for (int i = 0; i < 2; ++i)
			{
				for (int j = 0; j < 2; ++j)
				{
					p.x = radius * (-0.35 + 0.7 * i);
					p.y = radius * (-0.35 + 0.7 * j);
					subpulls[2*i + j].location = p;
					subpulls[2*i + j].diameter = radius * 0.67; 
				}
			}
			break;
		case SQUARE_SIXTEEN_TEMPLATE:
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					p.x = radius * (-0.52 + 0.35 * i);
					p.y = radius * (-0.52 + 0.35 * j);
					subpulls[4*i + j].location = p;
					subpulls[4*i + j].diameter = radius * 0.34; 
				}
			}
			break;
		case BUNDLE_NINETEEN_TEMPLATE:
		{
			int k = 0;
			for (int i = -2; i <= 2; ++i)
			{
				for (int j = -2; j <= 2; ++j)
				{
					int absj = (j < 0 ? -j : j);
					p.x = radius * (i + (absj%2)*0.5) * 0.4;
					p.y = radius * (j * sqrt3/2) * 0.4;
					if (p.x*p.x+p.y*p.y >= 1) continue;
					if (k >= 19) continue;
					subpulls[k].location = p;
					subpulls[k].diameter = radius * 0.4; 
					++k;
				}
			}
			break;
		}
		case X_NINE_TEMPLATE:
			for (int i = 0; i < 5; ++i)
			{
				p.x = radius * (-0.8 + 0.4 * i);
				p.y = 0.0;
				subpulls[i].location = p;
				subpulls[i].diameter = radius * 0.39; 
			}
			for (int i = 0; i < 2; ++i)
			{
				p.x = 0.0;
				p.y = radius * (-0.8 + 0.4 * i);
				subpulls[i+5].location = p;
				subpulls[i+5].diameter = radius * 0.39; 
			}
			for (int i = 3; i < 5; ++i)
			{
				p.x = 0.0;
				p.y = radius * (-0.8 + 0.4 * i);
				subpulls[i+4].location = p;
				subpulls[i+4].diameter = radius * 0.39; 
			}
			break;
	}	
}


void PullTemplate :: initializeSubpulls()
{
	Point p;
	p.x = p.y = p.z = 0.0;

	switch (type)
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
		case CASED_CIRCLE_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 1.99, 0));
			break;	
		case LINE_THREE_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			for (int i = 0; i < 3; ++i)
			{
				subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.60, 0));
			}
			break;	
		case LINE_FIVE_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			for (int i = 0; i < 5; ++i)
			{
				subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.39, 0));
			}
			break;			
		case CIRCLE_FOUR_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			for (int i = 0; i < 4; ++i)
			{
				subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.8, 0));
			}
			break;			
		case CIRCLE_SIX_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			for (int i = 0; i <= 6; ++i)
			{
				subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.8, 0));
			}
			break;			
		case CIRCLE_TWELVE_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			for (int i = 0; i <= 12; ++i)
			{
				subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.8, 0));
			}
			break;			
		case SQUARE_SIXTEEN_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			for (int i = 0; i < 16; ++i)
			{
				subpulls.push_back(SubpullTemplate(SQUARE_SHAPE, p, 0.49, 0));
			}
			break;
		case SQUARE_FOUR_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			for (int i = 0; i < 4; ++i)
			{
				subpulls.push_back(SubpullTemplate(SQUARE_SHAPE, p, 0.49, 0));
			}
			break;
		case BUNDLE_NINETEEN_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			for (int i = 0; i < 19; ++i)
			{
				subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.49, 0));
			}
			break;
		case X_NINE_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			for (int i = 0; i < 5; ++i)
			{
				subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.39, 0));
			}
			for (int i = 0; i < 5; ++i)
			{
				if (i == 2)
					continue;
				subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.39, 0));
			}
			break;	
	}
}


void PullTemplate :: setCasingThickness(float thickness)
{
	this->casingThickness = thickness;
	updateSubpulls();
}

float PullTemplate :: getCasingThickness()
{
	return this->casingThickness;
}


