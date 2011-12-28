
#include "pickuptemplate.h"
#include <stdio.h>

PickupTemplate :: PickupTemplate(int t)
{
	char* tmp;

	this->type = t;

	// set parameters
	switch (this->type)
	{
		case VERTICALS_AND_HORIZONTALS_PICKUP_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Column count:");
			parameterNames.push_back(tmp);
			parameterValues.push_back(14);
			break;
		case VERTICALS_PICKUP_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Column count:");
			parameterNames.push_back(tmp);
			parameterValues.push_back(14);
			break;
		case MURRINE_COLUMN_PICKUP_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Column count:");
			parameterNames.push_back(tmp);
                        parameterValues.push_back(14);
			break;
		case RETICELLO_VERTICAL_HORIZONTAL_PICKUP_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Column count:");
			parameterNames.push_back(tmp);
                        parameterValues.push_back(14);
			break;
                case VERTICAL_HORIZONTAL_VERTICAL_PICKUP_TEMPLATE:
                        tmp = new char[100];
                        sprintf(tmp, "Column count:");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(14);
                        break;
                case VERTICAL_WITH_LIP_WRAP_PICKUP_TEMPLATE:
                        tmp = new char[100];
                        sprintf(tmp, "Column count:");
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
		case MURRINE_COLUMN_PICKUP_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameterValues[0], 1);
			for (int i = 0; i < parameterValues[0]-1; ++i)
			{
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				subtemps.push_back(new SubpickupTemplate(p, VERTICAL_ORIENTATION, 2.0, width-0.0001, 
					CIRCLE_SHAPE, 0));
			}
			for (int i = 0; i < parameterValues[0]; ++i)
			{
				p.x = 1.0 - width / 2;
				p.y = -1.0 + width / 2 + width * i;
				p.z = -width/2;
				subtemps.push_back(new SubpickupTemplate(p, MURRINE_ORIENTATION, 0.2, width-0.0001, 
					SQUARE_SHAPE, 1));
			}
			break;
                case RETICELLO_VERTICAL_HORIZONTAL_PICKUP_TEMPLATE:
                        p.x = p.y = p.z = 0.0;
                        width = 2.0 / MAX(parameterValues[0], 1);
                        for (int i = 0; i < parameterValues[0]; ++i)
                        {
                                p.x = -1.0 + width / 2 + width * i;
                                p.y = -1.0;
                                subtemps.push_back(new SubpickupTemplate(p, VERTICAL_ORIENTATION, 2.0, width-0.0001,
                                        SQUARE_SHAPE, 0));
                        }
                        p.x = p.y = p.z = 0.0;
                        width = 2.0 / MAX(parameterValues[0], 1);
                        for (int i = 0; i < parameterValues[0]-1; ++i)
                        {
                                p.x = -1.0;
                                p.y = -1.0 + width / 2 + width * i;
				p.z = -width/2;
                                subtemps.push_back(new SubpickupTemplate(p, HORIZONTAL_ORIENTATION, 2.0, width-0.0001,
                                        SQUARE_SHAPE, 1));
                        }
			break;
		case VERTICALS_PICKUP_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameterValues[0], 1);
			for (int i = 0; i < parameterValues[0]; ++i)
			{
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				subtemps.push_back(new SubpickupTemplate(p, VERTICAL_ORIENTATION, 2.0, width-0.0001, 
					SQUARE_SHAPE, 0));
			}
			break;
                case VERTICAL_WITH_LIP_WRAP_PICKUP_TEMPLATE:
                        p.x = p.y = p.z = 0.0;
                        width = 2.0 / MAX(parameterValues[0]-1, 1);
			p.x = -1.0;
			p.y = 1.0 - width/2;
			subtemps.push_back(new SubpickupTemplate(p, HORIZONTAL_ORIENTATION, 2.0, width, SQUARE_SHAPE, 0));
                        for (int i = 0; i < parameterValues[0]-1; ++i)
                        {
                                p.x = -1.0 + width / 2 + width * i;
                                p.y = -1.0;
                                subtemps.push_back(new SubpickupTemplate(p, VERTICAL_ORIENTATION, 2.0 - width, width-0.0001,
                                        SQUARE_SHAPE, 1));
                        }
                        break;
		case VERTICALS_AND_HORIZONTALS_PICKUP_TEMPLATE:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameterValues[0], 1);
			for (int i = 0; i < parameterValues[0]; ++i)
			{
				p.x = -1.0 + width / 2 + width * i;
				p.y = 0.0;
				subtemps.push_back(new SubpickupTemplate(p, VERTICAL_ORIENTATION, 1.0, width-0.0001, 
					SQUARE_SHAPE, 0));
			}
			for (int i = 0; i < parameterValues[0]/2; ++i)
			{
				p.x = -1.0;
				p.y = -1.0 + width / 2 + width * i;
				subtemps.push_back(new SubpickupTemplate(p, HORIZONTAL_ORIENTATION, 2.0, width-0.0001, 
					SQUARE_SHAPE, 1));
			}
			break;
		case VERTICAL_HORIZONTAL_VERTICAL_PICKUP_TEMPLATE:
                        p.x = p.y = p.z = 0.0;
                        width = 2.0 / MAX(parameterValues[0], 1);
                        for (int i = 0; i < parameterValues[0]; ++i)
                        {
                                p.x = -1.0 + width / 2 + width * i;
                                p.y = -1.0;
                                subtemps.push_back(new SubpickupTemplate(p, VERTICAL_ORIENTATION, 0.75, width-0.0001,
                                        SQUARE_SHAPE, 0));
                        }
                        for (int i = 0; i < parameterValues[0]; ++i)
                        {
                                p.x = -1.0 + width / 2 + width * i;
                                p.y = 0.25;
                                subtemps.push_back(new SubpickupTemplate(p, VERTICAL_ORIENTATION, 0.75, width-0.0001,
                                        SQUARE_SHAPE, 2));
                        }
                        width = 0.5 / MAX(parameterValues[0]/3, 1);
                        for (int i = 0; i < parameterValues[0]/3; ++i)
                        {
                                p.x = -1.0;
                                p.y = -0.25 + width / 2 + width * i;
                                subtemps.push_back(new SubpickupTemplate(p, HORIZONTAL_ORIENTATION, 2.0, width-0.0001,
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


