
#include "pickuptemplate.h"
#include <stdio.h>

PickupTemplate :: PickupTemplate(int t)
{
	char* tmp;

	this->type = t;

	// set parameters
	switch (this->type)
	{
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
			parameterValues.push_back(14);
			break;
		case MURRINE_SQUARE_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Column count");
			parameterNames.push_back(tmp);
			parameterValues.push_back(14);
			break;
	}

	computeSubpulls();
}

PickupTemplate* PickupTemplate :: copy()
{
	PickupTemplate* c = new PickupTemplate(this->type);

	c->parameterValues.clear();
	for (unsigned int i = 0; i < this->parameterValues.size(); ++i)
	{
		c->parameterValues.push_back(this->parameterValues[i]);
	}
	c->computeSubpulls();

	return c;
}

void PickupTemplate :: computeSubpulls()
{
	for (unsigned int i = 0; i < subpulls.size(); ++i)
		delete subpulls[i];
	subpulls.clear();

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
				p.y = -0.99;
				subpulls.push_back(new SubpickupTemplate(p, VERTICAL_ORIENTATION, 1.99, width - 0.001, 
					CIRCLE_SHAPE, 0));
			}
			for (int i = 0; i < parameterValues[0]; ++i)
			{
				p.x = 1.0 - width / 2;
				p.y = -0.99 + width / 2 + width * i;
				subpulls.push_back(new SubpickupTemplate(p, MURRINE_ORIENTATION, 0.4, width - 0.001, 
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
					subpulls.push_back(new SubpickupTemplate(p, MURRINE_ORIENTATION, 0.4,
						width - 0.01, SQUARE_SHAPE, 0));
				}
			}
			break;
		case VERTICALS_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameterValues[0], 1);
			for (int i = 0; i < parameterValues[0]; ++i)
			{
				p.x = -1.0 + width / 2 + width * i;
				p.y = -0.99;
				subpulls.push_back(new SubpickupTemplate(p, VERTICAL_ORIENTATION, 1.99, width - 0.001, 
					SQUARE_SHAPE, 0));
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
	computeSubpulls();
}


