#include <math.h>
#include "pulltemplate.h"

PullTemplate :: PullTemplate(int type)
{
	this->type = type;
	this->casingThickness = 0.01;

	initializeTemplate();
	updateSubtemps(); // need to change locations of subtemps, but nothing else
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
		case HORIZONTAL_LINE_PULL_TEMPLATE:
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
		case CIRCLE_PULL_TEMPLATE:
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
			break;			
		}
		case SURROUND_CIRCLE_PULL_TEMPLATE:
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
		case SQUARE_PULL_TEMPLATE:
		{
			if (this->shape == CIRCLE_SHAPE)
				radius *= 1 / SQRT_TWO* 1.17;

			int count = parameterValues[0];
			float littleRadius = radius / count;

			for (int i = 0; i < count; ++i)
			{
				for (int j = 0; j < count; ++j)
				{
					p.x = -radius + littleRadius + 2 * littleRadius * i;
					p.y = -radius + littleRadius + 2 * littleRadius * j;
					subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, 2 * littleRadius, 0));
				}
			}
			break;
		}
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
					p.y = radius * (j * SQRT_THREE/2) * 0.4;
					if (p.x*p.x+p.y*p.y >= 1) continue;
					if (k >= 19) continue;
					subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, radius * 0.4, 0));
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
				subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, radius * 0.4, 0));
			}
			for (int i = 0; i < 2; ++i)
			{
				p.x = 0.0;
				p.y = radius * (-0.8 + 0.4 * i);
				subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, radius * 0.4, 0));
			}
			for (int i = 3; i < 5; ++i)
			{
				p.x = 0.0;
				p.y = radius * (-0.8 + 0.4 * i);
				subtemps.push_back(SubpullTemplate(CIRCLE_SHAPE, p, radius * 0.4, 0));
			}
			break;
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
			this->shape = SQUARE_SHAPE;
			this->base = true;
			break;
		case CASED_CIRCLE_PULL_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			this->base = false;
			this->casingThickness = 0.2;
			break;	
		case CASED_SQUARE_PULL_TEMPLATE:
			this->shape = SQUARE_SHAPE;
			this->base = false;
			this->casingThickness = 0.2;
			break;	
		case HORIZONTAL_LINE_PULL_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			this->base = false;
                        tmp = new char[100];
                        sprintf(tmp, "Count");
			this->parameterNames.push_back(tmp);
			this->parameterValues.push_back(3);
			break;	
		case CIRCLE_PULL_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
                        tmp = new char[100];
                        sprintf(tmp, "Count");
			this->parameterNames.push_back(tmp);
			this->parameterValues.push_back(4);
			this->base = false;
			break;			
		case SURROUND_CIRCLE_PULL_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
                        tmp = new char[100];
                        sprintf(tmp, "Count");
			this->parameterNames.push_back(tmp);
			this->parameterValues.push_back(8);
			this->base = false;
			break;			
		case SQUARE_PULL_TEMPLATE:
			this->shape = SQUARE_SHAPE;
                        tmp = new char[100];
                        sprintf(tmp, "Count");
			this->parameterNames.push_back(tmp);
			this->parameterValues.push_back(2);
			this->base = false;
			break;
		case BUNDLE_NINETEEN_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			this->base = false;
			break;
		case X_NINE_TEMPLATE:
			this->shape = CIRCLE_SHAPE;
			this->base = false;
			break;	
	}
}




