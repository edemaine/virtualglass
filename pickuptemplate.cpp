
#include "pickuptemplate.h"
#include <stdio.h>

PickupTemplate :: PickupTemplate(int t)
{
	char* tmp;

	this->type = t;

	// set parameters
	switch (this->type)
	{
		case VERTICALS_AND_HORIZONTALS_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Column count");
			parameterNames.push_back(tmp);
			parameterValues.push_back(14);
			break;
		case VERTICALS_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Column count");
			parameterNames.push_back(tmp);
			parameterValues.push_back(14);
			break;
		case MURRINE_COLUMN_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Column count");
			parameterNames.push_back(tmp);
			parameterValues.push_back(10);
			break;
		case MURRINE_SQUARE_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Column count");
			parameterNames.push_back(tmp);
			parameterValues.push_back(14);
			break;
	}

	computeSubtemps();
}

PickupTemplate* PickupTemplate :: copy()
{
	PickupTemplate* c = new PickupTemplate(this->type);

	c->parameterValues.clear();
	for (unsigned int i = 0; i < this->parameterValues.size(); ++i)
	{
		c->parameterValues.push_back(this->parameterValues[i]);
	}
	c->computeSubtemps();

	return c;
}

void PickupTemplate :: computeSubtemps()
{
	for (unsigned int i = 0; i < subtemps.size(); ++i)
		delete subtemps[i];
	subtemps.clear();

	Point p;
	float width;

	// initialize SubpickupTemplates with the right data
	switch (this->type)
	{
		case MURRINE_COLUMN_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameterValues[0], 1);
			for (int i = 0; i < parameterValues[0]-1; ++i)
			{
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				subtemps.push_back(new SubpickupTemplate(p, VERTICAL_ORIENTATION, 2.0, width-0.00001, 
					CIRCLE_SHAPE, 0));
			}
			for (int i = 0; i < parameterValues[0]; ++i)
			{
				p.x = 1.0 - width / 2;
				p.y = -1.0 + width / 2 + width * i;
				p.z = -width/2;
				subtemps.push_back(new SubpickupTemplate(p, MURRINE_ORIENTATION, width-0.00001, width-0.00001, 
					SQUARE_SHAPE, 1));
			}
			break;
		case MURRINE_SQUARE_TEMPLATE:
			width = 2.0 / MAX(parameterValues[0], 1);
			for (int i = 0; i < parameterValues[0]; ++i)
			{
				for (int j = 0; j < parameterValues[0]; ++j)
				{
					p.x = -1.0 + width / 2 + width * i;
					p.y = -1.0 + width / 2 + width * j;
					p.z = width / 2;
					subtemps.push_back(new SubpickupTemplate(p, MURRINE_ORIENTATION, 0.2,
						width-0.00001, SQUARE_SHAPE, 0));
				}
			}
			break;
		case VERTICALS_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameterValues[0], 1);
			for (int i = 0; i < parameterValues[0]; ++i)
			{
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				subtemps.push_back(new SubpickupTemplate(p, VERTICAL_ORIENTATION, 2.0, width-0.00001, 
					SQUARE_SHAPE, 0));
			}
			break;
		case VERTICALS_AND_HORIZONTALS_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameterValues[0], 1);
			for (int i = 0; i < parameterValues[0]; ++i)
			{
				p.x = -1.0 + width / 2 + width * i;
				p.y = 0.0;
				subtemps.push_back(new SubpickupTemplate(p, VERTICAL_ORIENTATION, 1.0, width-0.00001, 
					SQUARE_SHAPE, 0));
			}
			for (int i = 0; i < parameterValues[0]/2; ++i)
			{
				p.x = -1.0;
				p.y = -1.0 + width / 2 + width * i;
				subtemps.push_back(new SubpickupTemplate(p, HORIZONTAL_ORIENTATION, 2.0, width-0.00001, 
					SQUARE_SHAPE, 1));
			}
			break;
	}
}

char* PickupTemplate :: getParameterName(int param)
{
	return parameterNames[param];
}

int PickupTemplate :: getParameter(int param)
{
	return parameterValues[param];
}

void PickupTemplate :: setParameter(int param, int newValue)
{
	parameterValues[param] = newValue;
	computeSubtemps();
}


