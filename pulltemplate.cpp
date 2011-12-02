#include <math.h>
#include "pulltemplate.h"
#define sqrt2 1.41421356
#define sqrt3 1.73205081
#define pi 3.1415926535

PullTemplate :: PullTemplate(int type)
{
	this->type = type;
	this->casingThickness = 0.01;

	initializeSubtemps(); // need to just allocate the right number of subtemps with casing thickness 0
	updateSubtemps(); // need to change locations of subtemps, but nothing else
}

bool PullTemplate :: isBase()
{
	return base;
}

void PullTemplate :: setCasingThickness(float t)
{
	casingThickness = t;
	updateSubtemps();
}

float PullTemplate :: getCasingThickness()
{
	return casingThickness;
}

int PullTemplate :: getShape()
{
	return this->shape;
}

void PullTemplate :: setShape(int s)
{
	this->shape = s;
	updateSubtemps();
}

void PullTemplate :: updateSubtemps()
{
	Point p;
	float radius = 1.0 - casingThickness;

	p.x = p.y = p.z = 0.0;
	switch (this->type)
	{
		case CASED_CIRCLE_PULL_TEMPLATE:
			subtemps[0].diameter = radius * 2; 
			break;	
		case CASED_SQUARE_PULL_TEMPLATE:
			if (this->shape == CIRCLE_SHAPE)
			{
				radius *= 1.0 / pow(2, 0.5);		
			}			
			subtemps[0].diameter = radius * 2; 
			break;	
		case HORIZONTAL_LINE_PULL_TEMPLATE:
			for (int i = 0; i < 3; ++i)
			{
				p.x = radius * (-0.6666 + 0.6666 * i);
				subtemps[i].location = p;
				subtemps[i].diameter = radius * 0.65; 
			}
			break;	
		case CIRCLE_PULL_TEMPLATE:
			if (this->shape == CIRCLE_SHAPE)
				radius *= 0.8;
			for (int i = 0; i < 2; ++i)
			{
				for (int j = 0; j < 2; ++j)
				{
					p.x = radius * (-0.5 + 1 * i);
					p.y = radius * (-0.5 + 1 * j);
					subtemps[2*i + j].location = p;
					subtemps[2*i + j].diameter = radius;
				}
			}
			break;			
		case SURROUND_CIRCLE_PULL_TEMPLATE:
			for (int i = 0; i < 12; ++i)
			{
				p.x = radius * cos (2*pi*i/12) * 0.79;
				p.y = radius * sin (2*pi*i/12) * 0.79;
				subtemps[i].location = p;
				subtemps[i].diameter = radius * 1/2 * 0.79;
			}
			p.x = 0;
			p.y = 0;
			subtemps[12].location = p;
			subtemps[12].diameter = radius * 1*1.5 * 0.79;
			break;			
		case SQUARE_PULL_TEMPLATE:
			if (this->shape == CIRCLE_SHAPE)
				radius *= 1 / pow(2, 0.5);
			for (int i = 0; i < 2; ++i)
			{
				for (int j = 0; j < 2; ++j)
				{
					p.x = radius * (-0.5 + i);
					p.y = radius * (-0.5 + j);
					subtemps[2*i + j].location = p;
					subtemps[2*i + j].diameter = radius;
				}
			}
			break;
		case BUNDLE_NINETEEN_TEMPLATE:
		{
			if (this->shape == SQUARE_SHAPE)
				radius *= pow(2, 0.5);
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
					subtemps[k].location = p;
					subtemps[k].diameter = radius * 0.4; 
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
				subtemps[i].location = p;
				subtemps[i].diameter = radius * 0.39; 
			}
			for (int i = 0; i < 2; ++i)
			{
				p.x = 0.0;
				p.y = radius * (-0.8 + 0.4 * i);
				subtemps[i+5].location = p;
				subtemps[i+5].diameter = radius * 0.39; 
			}
			for (int i = 3; i < 5; ++i)
			{
				p.x = 0.0;
				p.y = radius * (-0.8 + 0.4 * i);
				subtemps[i+4].location = p;
				subtemps[i+4].diameter = radius * 0.39; 
			}
			break;
	}	
}


void PullTemplate :: initializeSubtemps()
{
	Point p;
	p.x = p.y = p.z = 0.0;

	char* tmp;
	switch (type)
	{
		case CIRCLE_BASE_PULL_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			this->base = true;
			break;
		case SQUARE_BASE_PULL_TEMPLATE:
			this->shape = SQUARE_SHAPE;
			this->base = true;
			break;
		case CASED_CIRCLE_PULL_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			this->base = false;
			this->casingThickness = 0.2;
			subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 1.99, 0));
			break;	
		case CASED_SQUARE_PULL_TEMPLATE:
			this->shape = SQUARE_SHAPE;
			this->base = false;
			this->casingThickness = 0.2;
			subtemps.push_back(SubpullTemplate(SQUARE_SHAPE, p, 1.99, 0));
			break;	
		case HORIZONTAL_LINE_PULL_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			this->base = false;
                        tmp = new char[100];
                        sprintf(tmp, "Count");
			this->parameterNames.push_back(tmp);
			this->parameterValues.push_back(3);
			for (int i = 0; i < 3; ++i)
			{
				subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.60, 0));
			}
			break;	
		case CIRCLE_PULL_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			this->base = false;
			for (int i = 0; i < 4; ++i)
			{
				subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.8, 0));
			}
			break;			
		case SURROUND_CIRCLE_PULL_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			this->base = false;
			for (int i = 0; i <= 12; ++i)
			{
				subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.8, 0));
			}
			break;			
		case SQUARE_PULL_TEMPLATE:
			this->shape = SQUARE_SHAPE;
			this->base = false;
			for (int i = 0; i < 16; ++i)
			{
				subtemps.push_back(SubpullTemplate(SQUARE_SHAPE, p, 0.49, 0));
			}
			break;
		case BUNDLE_NINETEEN_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			this->base = false;
			for (int i = 0; i < 19; ++i)
			{
				subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.49, 0));
			}
			break;
		case X_NINE_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			this->base = false;
			for (int i = 0; i < 5; ++i)
			{
				subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.39, 0));
			}
			for (int i = 0; i < 5; ++i)
			{
				if (i == 2)
					continue;
				subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.39, 0));
			}
			break;	
	}
}




