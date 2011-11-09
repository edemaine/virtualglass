
#include "pulltemplate.h"

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
		case ONE_COLORED_CASING_TEMPLATE:
			subpulls[0].diameter = radius * 1.0; 
			subpulls[1].diameter = radius * 1.99; 
			break;	
		case SQUARE_SIXTEEN_SQUARES_TEMPLATE:
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
		case SQUARE_FOUR_SQUARES_TEMPLATE:
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
		case LINE_THREE_CIRCLES_TEMPLATE:
			for (int i = 0; i < 3; ++i)
			{
				p.x = radius * (-0.6666 + 0.6666 * i);
				subpulls[i].location = p;
				subpulls[i].diameter = radius * 0.65; 
			}
			break;	
		case LINE_FIVE_CIRCLES_TEMPLATE:
			for (int i = 0; i < 5; ++i)
			{
				p.x = radius * (-0.8 + 0.4 * i);
				subpulls[i].location = p;
				subpulls[i].diameter = radius * 0.39; 
			}
			break;			
		case SQUARE_FOUR_CIRCLES_TEMPLATE:
			for (int i = 0; i < 2; ++i)
			{
				for (int j = 0; j < 2; ++j)
				{
					p.x = radius * (-0.4 + 0.8 * i);
					p.y = radius * (-0.4 + 0.8 * j);
					subpulls[2*i + j].location = p;
					subpulls[2*i + j].diameter = radius * 0.79; 
				}
			}
			break;			
		case X_NINE_CIRCLES_TEMPLATE:
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
		case SQUARE_SIXTEEN_SQUARES_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			for (int i = 0; i < 16; ++i)
			{
				subpulls.push_back(SubpullTemplate(SQUARE_SHAPE, p, 0.49, 0));
			}
			break;
		case SQUARE_FOUR_SQUARES_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			for (int i = 0; i < 4; ++i)
			{
				subpulls.push_back(SubpullTemplate(SQUARE_SHAPE, p, 0.49, 0));
			}
			break;
		case LINE_THREE_CIRCLES_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			for (int i = 0; i < 3; ++i)
			{
				subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.60, 0));
			}
			break;	
		case LINE_FIVE_CIRCLES_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			for (int i = 0; i < 5; ++i)
			{
				subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.39, 0));
			}
			break;			
		case SQUARE_FOUR_CIRCLES_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			for (int i = 0; i < 4; ++i)
			{
				subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 0.59, 0));
			}
			break;			
		case X_NINE_CIRCLES_TEMPLATE:
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
		case ONE_COLORED_CASING_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 1.0, 0));
			subpulls.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 1.99, 1));
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


