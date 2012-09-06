
#include "pickupplan.h"
#include <cstdio>

PickupPlan :: PickupPlan(int templateType) {

	defaultSubplan = new PullPlan(BASE_CIRCLE_PULL_TEMPLATE);
	defaultGlassColor = new GlassColor();
	casingGlassColor = underlayGlassColor = overlayGlassColor = defaultGlassColor;

	this->templateType = -1; // to ensure setTemplateType goes through
	setTemplateType(templateType);
}

PickupPlan* PickupPlan :: copy() const {
	
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

	c->overlayGlassColor = this->overlayGlassColor;
	c->underlayGlassColor = this->underlayGlassColor;

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
		case MURRINE_PICKUP_TEMPLATE:
                        width = 2.0 / MAX(parameterValues[0], 1);
			for (int r = 0; r < parameterValues[0]; ++r)
			{
				for (int c = 0; c < parameterValues[0]; ++c)
				{
					p.x = -1.0 + width / 2 + width * r;
					p.y = -1.0 + width / 2 + width * c;
					p.z = -width/2;
					pushNewSubplan(&newSubs, p, MURRINE_ORIENTATION, parameterValues[1]*0.005 + 0.005, width-0.0001,
						SQUARE_SHAPE, 1);
				}
			}
                        break;
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
		case MURRINE_PICKUP_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Row/Column count:");
			parameterNames.push_back(tmp);
			parameterValues.push_back(10);
			tmp = new char[100];
			sprintf(tmp, "Thickness:");
			parameterNames.push_back(tmp);
			parameterValues.push_back(10);
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

unsigned int PickupPlan :: getParameterCount()
{
	return parameterValues.size();
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


PickupPlan *deep_copy(const PickupPlan *_pickup) {
	assert(_pickup);
	PickupPlan *pickup = _pickup->copy();
	for (vector< SubpickupTemplate >::iterator s = pickup->subs.begin(); s != pickup->subs.end(); ++s) {
		s->plan = deep_copy(s->plan);
	}
	return pickup;
}

void deep_delete(PickupPlan *pickup) {
	assert(pickup);
	for (vector< SubpickupTemplate >::iterator s = pickup->subs.begin(); s != pickup->subs.end(); ++s) {
		delete s->plan;
		s->plan = NULL;
	}
	delete pickup;
}
