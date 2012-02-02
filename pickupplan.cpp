
#include "pickupplan.h"
#include <cstdio>

PickupPlan :: PickupPlan(int templateType) {

	Color* defaultColor;
	defaultColor = new Color();
	defaultColor->r = defaultColor->g = defaultColor->b = 1.0;
	defaultColor->a = 0.0;

	defaultSubplan = new PullPlan(CIRCLE_BASE_PULL_TEMPLATE);
	overlayColor = defaultColor;
	underlayColor = defaultColor;
	useUnderlay = false;

	this->templateType = -1; // to ensure setTemplateType goes through
	setTemplateType(templateType);
}

PickupPlan* PickupPlan :: copy() {    
	
	PickupPlan* c = new PickupPlan(this->templateType);

        for (unsigned int i = 0; i < this->parameterNames.size(); ++i)
        {
                c->parameterValues[i] = this->parameterValues[i];
        }
	c->updateSubs();

	for (unsigned int i = 0; i < this->subs.size(); ++i)
	{
		c->subs[i].plan = this->subs[i].plan;
	}


	c->overlayColor = this->overlayColor;
	c->underlayColor = this->underlayColor;
	c->useUnderlay = this->useUnderlay;

	return c;
}


void PickupPlan :: pushNewSubplan(vector<SubpickupTemplate>* newSubs,
    Point location, int orientation, float length, float width, int shape, int group) {

        if (newSubs->size() < subs.size())
        {
		newSubs->push_back(SubpickupTemplate(subs[newSubs->size()].plan,
		location, orientation, length, width, shape, group));
        }
        else // you've run out of existing subplans copy from
        {
		newSubs->push_back(SubpickupTemplate(defaultSubplan,
		location, orientation, length, width, shape, group));
        }
}

void PickupPlan :: updateSubs() {

	vector<SubpickupTemplate> newSubs;

        Point p;
        float width;
        switch (this->templateType) {
                case MURRINE_COLUMN_PICKUP_TEMPLATE:
                        width = 2.0 / MAX(parameterValues[0], 1);
                        for (int i = 0; i < parameterValues[0]-1; ++i) {
                                p.x = 1.0 - width / 2;
                                p.y = -1.0 + width / 2 + width * (parameterValues[0]- 1 - i);
                                p.z = -width/2;
                                pushNewSubplan(&newSubs, p, MURRINE_ORIENTATION, width, width-0.0001,
                                        SQUARE_SHAPE, 1);
                                p.x = -1.0 + width / 2 + width * i;
                                p.y = -1.0;
                                p.z = 0.0;
                                pushNewSubplan(&newSubs, p, VERTICAL_ORIENTATION, 2.0, width-0.0001,
                                        CIRCLE_SHAPE, 0);
                        }
                        p.x = 1.0 - width / 2;
                        p.y = -1.0 + width / 2;
                        p.z = -width/2;
                        pushNewSubplan(&newSubs, p, MURRINE_ORIENTATION, width, width-0.0001,
                                SQUARE_SHAPE, 1);
                        break;
                case MURRINE_ROW_PICKUP_TEMPLATE:
                        p.x = p.y = p.z = 0.0;
                        width = 2.0 / MAX(parameterValues[0], 1);
                        for (int i = 0; i < parameterValues[0]; ++i) {
                                p.x = -1.0 + width / 2 + width * i;
                                p.y = -1.0;
				p.z = 0.0;
                                pushNewSubplan(&newSubs, p, VERTICAL_ORIENTATION, 1.0 - width/2, width-0.0001,
                                        CIRCLE_SHAPE, 0);
                                p.y = 0.0;
                                p.x = -1.0 + width / 2 + width * i;
                                p.z = -width/2;
                                pushNewSubplan(&newSubs, p, MURRINE_ORIENTATION, width, width-0.0001,
                                        SQUARE_SHAPE, 1);
                                p.x = -1.0 + width / 2 + width * i;
                                p.y = width/2;
				p.z = 0.0;
                                pushNewSubplan(&newSubs, p, VERTICAL_ORIENTATION, 1.0 - width/2, width-0.0001,
                                        CIRCLE_SHAPE, 2);
                        }
                        break;
                case RETICELLO_VERTICAL_HORIZONTAL_PICKUP_TEMPLATE:
                        p.x = p.y = p.z = 0.0;
                        width = 2.0 / MAX(parameterValues[0], 1);
                        for (int i = 0; i < parameterValues[0]-1; ++i) {
                                p.x = -1.0 + width / 2 + width * i;
                                p.y = -1.0;
				p.z = 0.0;
                                pushNewSubplan(&newSubs, p, VERTICAL_ORIENTATION, 2.0, width-0.0001,
                                        SQUARE_SHAPE, 0);
                                p.x = -1.0;
                                p.y = 1.0 - (1.5 * width + width * i);
                                p.z = -width/2;
                                pushNewSubplan(&newSubs, p, HORIZONTAL_ORIENTATION, 2.0, width-0.0001,
                                        SQUARE_SHAPE, 1);
                        }
			p.x = -1.0 + width / 2 + width * (parameterValues[0]-1);
			p.y = -1.0;
			p.z = 0.0;
			pushNewSubplan(&newSubs, p, VERTICAL_ORIENTATION, 2.0, width-0.0001,
				SQUARE_SHAPE, 0);
                        break;
                case VERTICALS_PICKUP_TEMPLATE:
                        p.x = p.y = p.z = 0.0;
                        width = 2.0 / MAX(parameterValues[0], 1);
                        for (int i = 0; i < parameterValues[0]; ++i) {
                                p.x = -1.0 + width / 2 + width * i;
                                p.y = -1.0;
                                pushNewSubplan(&newSubs, p, VERTICAL_ORIENTATION, 2.0, width-0.0001,
                                        SQUARE_SHAPE, 0);
                        }
                        break;
                case VERTICAL_WITH_LIP_WRAP_PICKUP_TEMPLATE:
                        p.x = p.y = p.z = 0.0;
                        width = 2.0 / MAX(parameterValues[0], 1);
                        p.x = -1.0;
                        p.y = 1.0 - width/2;
                        pushNewSubplan(&newSubs, p, HORIZONTAL_ORIENTATION, 2.0, width, SQUARE_SHAPE, 0);
                        for (int i = 0; i < parameterValues[0]; ++i) {
                                p.x = -1.0 + width / 2 + width * i;
                                p.y = -1.0;
                                pushNewSubplan(&newSubs, p, VERTICAL_ORIENTATION, 2.0 - width, width-0.0001,
                                        SQUARE_SHAPE, 1);
                        }
                        break;
                case VERTICALS_AND_HORIZONTALS_PICKUP_TEMPLATE:
		{
                        float verticals_width = 2.0 / MAX(parameterValues[0], 1);
                        float horizontals_width = 1.0 / MAX(parameterValues[0]/2, 1);
                        for (int i = 0; i < parameterValues[0]; ++i) {
                                p.x = -1.0 + verticals_width / 2 + verticals_width * i;
                                p.y = 0.0;
				p.z = 0.0;
                                pushNewSubplan(&newSubs, p, VERTICAL_ORIENTATION, 1.0, verticals_width-0.0001,
                                        SQUARE_SHAPE, 0);
				if (i % 2 == 1) {
					p.x = -1.0;
					p.y = -(horizontals_width / 2 + horizontals_width * (i-1) / 2);
					p.z = 0.0;
					pushNewSubplan(&newSubs, p, HORIZONTAL_ORIENTATION, 2.0,
					horizontals_width-0.0001, SQUARE_SHAPE, 1);
				}
			}
			break;
        	}
                case VERTICAL_HORIZONTAL_VERTICAL_PICKUP_TEMPLATE:
        	{
			float verticals_width = 2.0 / MAX(parameterValues[0], 1);
			float horizontals_width = 0.5 / MAX(parameterValues[0]/3, 1);
                        for (int i = 0; i < parameterValues[0]; ++i) {
                                p.x = -1.0 + verticals_width / 2 + verticals_width * i;
                                p.y = -1.0;
				p.z = 0.0;
				pushNewSubplan(&newSubs, p, VERTICAL_ORIENTATION, 0.75,
					verticals_width-0.0001, SQUARE_SHAPE, 0);
                                p.x = -1.0 + verticals_width / 2 + verticals_width * i;
                                p.y = 0.25;
				p.z = 0.0;
                                pushNewSubplan(&newSubs, p, VERTICAL_ORIENTATION, 0.75,
					verticals_width-0.0001, SQUARE_SHAPE, 2);
				if (i % 3 == 2) {
					p.x = -1.0;
					p.y = 0.25 - horizontals_width / 2 - horizontals_width * (i-2) / 3;
					p.z = 0.0;
					pushNewSubplan(&newSubs, p, HORIZONTAL_ORIENTATION, 2.0, horizontals_width-0.0001,
					SQUARE_SHAPE, 1);
				}
                        }
                        break;
		}
        }

	subs = newSubs;
}


void PickupPlan :: setTemplateType(int templateType) {

	if (this->templateType == templateType)
		return;

	this->templateType = templateType;

	parameterNames.clear();
	parameterValues.clear();
	char* tmp;
	switch (this->templateType) {
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
                case MURRINE_ROW_PICKUP_TEMPLATE:
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

        subs.clear(); // don't carry over any of the current stuff
	updateSubs();
}

int PickupPlan :: getTemplateType() {

	return this->templateType;
}

char* PickupPlan :: getParameterName(int param) {

        return parameterNames[param];
}

int PickupPlan :: getParameter(int param) {

        return parameterValues[param];
}

void PickupPlan :: setParameter(int param, int newValue) {

        parameterValues[param] = newValue;
	updateSubs();
}






