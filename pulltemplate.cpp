#include <math.h>
#include "pulltemplate.h"
#include <stdio.h>

PullTemplate :: PullTemplate(int type)
{
	this->type = type;
	this->casingThickness = 0.01;

	initializeTemplate();
	updateSubtemps(); // need to change locations of subtemps, but nothing else
}

PullTemplate* PullTemplate :: copy()
{
	PullTemplate* c = new PullTemplate(this->type);
	c->setShape(this->shape);
	c->setCasingThickness(this->casingThickness);	
	for (unsigned int i = 0; i < this->parameterNames.size(); ++i)
	{
		c->setParameter(i, this->parameterValues[i]);
	} 

	return c;
}

void PullTemplate :: setParameter(int p, int v)
{
	parameterValues[p] = v;
	updateSubtemps();
}

int PullTemplate :: getParameter(int p)
{
	return parameterValues[p];
}

char* PullTemplate :: getParameterName(int p)
{
	return parameterNames[p];
}

unsigned int PullTemplate :: getParameterCount()
{
	return parameterNames.size();
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

	subtemps.clear();
	p.x = p.y = p.z = 0.0;
	switch (this->type)
	{
		case CASED_CIRCLE_PULL_TEMPLATE:
			subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, radius * 2.0, 0));
			break;
		case CASED_SQUARE_PULL_TEMPLATE:
			if (this->shape == CIRCLE_SHAPE)
			{
				radius *= 1.0 / pow(2, 0.5);
			}
			subtemps.push_back(SubpullTemplate(SQUARE_SHAPE, p, radius * 2.0, 0));
			break;
		case HORIZONTAL_LINE_CIRCLE_PULL_TEMPLATE:
		{
			int count = parameterValues[0];
			for (int i = 0; i < count; ++i)
			{
				float littleRadius = (2 * radius / count) / 2;
				p.x = -radius + littleRadius + i * 2 * littleRadius;
				subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, littleRadius * 2.0, 0));
			}
			break;
		}
		case HORIZONTAL_LINE_SQUARE_PULL_TEMPLATE:
		{
			if (this->shape == CIRCLE_SHAPE)
				radius *= 0.9;

			int count = parameterValues[0];
			for (int i = 0; i < count; ++i)
			{
				float littleRadius = (2 * radius / count) / 2;
				p.x = -radius + littleRadius + i * 2 * littleRadius;
				subtemps.push_back(SubpullTemplate(SQUARE_SHAPE, p, littleRadius * 2.0, 0));
			}
			break;
		}
		case SURROUNDING_CIRCLE_PULL_TEMPLATE:
		{
			int count = parameterValues[0];
			float theta = TWO_PI / count;
			float k = sin(theta/2) / (1 + sin(theta/2));

			for (int i = 0; i < count; ++i)
			{
				p.x = (1.0 - k) * radius * cos(TWO_PI / count * i);
				p.y = (1.0 - k) * radius * sin(TWO_PI / count * i);
				subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 2 * k * radius, 0));
			}
			p.x = p.y = 0.0;
			subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, (1 - 2 * k) * 2 * radius, 0));
			break;
		}
		case CROSS_PULL_TEMPLATE:
		{
			int count = parameterValues[0] * 2 - 1;

			for (int i = 0; i < count; ++i)
			{
				float littleRadius = (2 * radius / count) / 2;
				p.x = -radius + littleRadius + i * 2 * littleRadius;
				p.y = 0.0;
				subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, littleRadius * 2.0, 0));
			}
			for (int i = 0; i < count; ++i)
			{
				if (i == (count - 1)/2)
					continue;
				float littleRadius = (2 * radius / count) / 2;
				p.x = 0.0;
				p.y = -radius + littleRadius + i * 2 * littleRadius;
				subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, littleRadius * 2.0, 0));
			}
			break;
		}
		case SQUARE_OF_CIRCLES_PULL_TEMPLATE:
		case SQUARE_OF_SQUARES_PULL_TEMPLATE:
		{
			if (this->shape == CIRCLE_SHAPE)
				radius *= 1 / SQRT_TWO;

			int count = parameterValues[0];
			float littleRadius = radius / count;

			for (int i = 0; i < count; ++i)
			{
				for (int j = 0; j < count; ++j)
				{
					p.x = -radius + littleRadius + 2 * littleRadius * i;
					p.y = -radius + littleRadius + 2 * littleRadius * j;
					if (this->type == SQUARE_OF_CIRCLES_PULL_TEMPLATE)
						subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 2 * littleRadius, 0));
					else
						subtemps.push_back(SubpullTemplate(SQUARE_SHAPE, p, 2 * littleRadius, 0));
				}
			}
			break;
		}
		case TRIPOD_PULL_TEMPLATE:
		{
			int count = parameterValues[0];
			float littleRadius = radius / (2 * count - 1);

			p.x = p.y = 0.0;
			subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 2 * littleRadius, 0));
			for (int theta = 0; theta < 3; ++theta)
			{
				for (int i = 1; i < count; ++i)
				{
					p.x = (littleRadius * 2 * i) * cos(TWO_PI / 3 * theta);
					p.y = (littleRadius * 2 * i) * sin(TWO_PI / 3 * theta); 
					subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, littleRadius * 2, 0));
				}
			}
			break;
		}
		case SURROUNDING_SQUARE_PULL_TEMPLATE:
		{	
			break;
		}
	}
}


void PullTemplate :: initializeTemplate()
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
			this->shape = CIRCLE_SHAPE;
			this->base = true;
			break;
		case AMORPHOUS_BASE_PULL_TEMPLATE:
			this->shape = AMORPHOUS_SHAPE;
			this->base = true;
			break;
		case CASED_CIRCLE_PULL_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			this->base = false;
			this->casingThickness = 0.2;
			break;
		case CASED_SQUARE_PULL_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			this->base = false;
			this->casingThickness = 0.2;
			break;
		case HORIZONTAL_LINE_CIRCLE_PULL_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			this->base = false;
			tmp = new char[100];
			sprintf(tmp, "Row count:");
			this->parameterNames.push_back(tmp);
			this->parameterValues.push_back(3);
			break;
		case HORIZONTAL_LINE_SQUARE_PULL_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			this->base = false;
			tmp = new char[100];
			sprintf(tmp, "Row count:");
			this->parameterNames.push_back(tmp);
			this->parameterValues.push_back(3);
			break;
		case SURROUNDING_CIRCLE_PULL_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			tmp = new char[100];
			sprintf(tmp, "Count:");
			this->parameterNames.push_back(tmp);
			this->parameterValues.push_back(8);
			this->base = false;
			break;
		case CROSS_PULL_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			tmp = new char[100];
			sprintf(tmp, "Radial count:");
			this->parameterNames.push_back(tmp);
			this->parameterValues.push_back(3);
			this->base = false;
			break;
		case SQUARE_OF_SQUARES_PULL_TEMPLATE:
		case SQUARE_OF_CIRCLES_PULL_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			tmp = new char[100];
			sprintf(tmp, "Row count:");
			this->parameterNames.push_back(tmp);
			this->parameterValues.push_back(4);
			this->base = false;
			break;
		case TRIPOD_PULL_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			tmp = new char[100];
			sprintf(tmp, "Radial count:");
			this->parameterNames.push_back(tmp);
			this->parameterValues.push_back(3);
			this->base = false;
			break;
	}
}




