
#include "constants.h"
#include "templateparameter.h"
#include "subpickuptemplate.h"
#include "pulltemplate.h"
#include "globalglass.h"
#include "pullplan.h"
#include "glasscolor.h"
#include "pickupplan.h"

PickupPlan :: PickupPlan(enum PickupTemplate::Type _type) {

	casingGlassColor = underlayGlassColor = overlayGlassColor = GlobalGlass::color();
	setTemplateType(_type, true);
}

PickupPlan* PickupPlan :: copy() const {
	
	PickupPlan* c = new PickupPlan(type);

	for (unsigned int i = 0; i < parameters.size(); ++i)
	{
		c->parameters[i] = parameters[i];
	}
	c->updateSubs();

	for (unsigned int i = 0; i < subs.size(); ++i)
	{
		c->subs[i].plan = subs[i].plan;
	}

	c->overlayGlassColor = overlayGlassColor;
	c->underlayGlassColor = underlayGlassColor;

	return c;
}


void PickupPlan :: pushNewSubplan(vector<SubpickupTemplate>* newSubs,
	Point location, enum PickupCaneOrientation ori, float length, float width, enum GeometricShape shape) {

	if (newSubs->size() < subs.size())
	{
		newSubs->push_back(SubpickupTemplate(subs[newSubs->size()].plan,
			location, ori, length, width, shape));
	}
	else // you've run out of existing subplans copy from
	{
		newSubs->push_back(SubpickupTemplate(GlobalGlass::circlePlan(),
			location, ori, length, width, shape));
	}
}

void PickupPlan :: updateSubs() {

	vector<SubpickupTemplate> newSubs;

	Point p;
	float width, length;
	switch (this->type) {
		case PickupTemplate::MURRINE:
			width = 2.0 / MAX(parameters[0].value, 1);
			length = parameters[1].value*0.005 + 0.005;
			for (int r = 0; r < parameters[0].value; ++r)
			{
				for (int c = 0; c < parameters[0].value; ++c)
				{
					p.x = -1.0 + width / 2 + width * r;
					p.y = -1.0 + width / 2 + width * c;
					p.z = -length/2;
					pushNewSubplan(&newSubs, p, MURRINE_PICKUP_CANE_ORIENTATION, 
						length, width-0.0001, SQUARE_SHAPE);
				}
			}
			break;
		case PickupTemplate::MURRINE_COLUMN:
			width = 2.0 / MAX(parameters[0].value, 1);
			for (int i = 0; i < parameters[0].value-1; ++i) {
				p.x = 1.0 - width / 2;
				p.y = -1.0 + width / 2 + width * (parameters[0].value - 1 - i);
				p.z = -width/2;
				pushNewSubplan(&newSubs, p, MURRINE_PICKUP_CANE_ORIENTATION, width, width-0.0001, SQUARE_SHAPE);
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				p.z = 0.0;
				pushNewSubplan(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 2.0, width-0.0001, CIRCLE_SHAPE);
			}
			p.x = 1.0 - width / 2;
			p.y = -1.0 + width / 2;
			p.z = -width/2;
			pushNewSubplan(&newSubs, p, MURRINE_PICKUP_CANE_ORIENTATION, width, width-0.0001, SQUARE_SHAPE);
			break;
		case PickupTemplate::MURRINE_ROW:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameters[0].value, 1);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				p.z = 0.0;
				pushNewSubplan(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 1.0 - width/2, 
					width-0.0001, CIRCLE_SHAPE);
				p.y = 0.0;
				p.x = -1.0 + width / 2 + width * i;
				p.z = -width/2;
				pushNewSubplan(&newSubs, p, MURRINE_PICKUP_CANE_ORIENTATION, width, 
					width-0.0001, SQUARE_SHAPE);
				p.x = -1.0 + width / 2 + width * i;
				p.y = width/2;
				p.z = 0.0;
				pushNewSubplan(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 1.0 - width/2, 
					width-0.0001, CIRCLE_SHAPE);
			}
			break;
		case PickupTemplate::RETICELLO_VERTICAL_HORIZONTAL:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameters[0].value, 1);
			for (int i = 0; i < parameters[0].value-1; ++i) {
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				p.z = 0.0;
				pushNewSubplan(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 2.0, width-0.0001, SQUARE_SHAPE);
				p.x = -1.0;
				p.y = 1.0 - (1.5 * width + width * i);
				p.z = -width/2;
				pushNewSubplan(&newSubs, p, HORIZONTAL_PICKUP_CANE_ORIENTATION, 2.0, width-0.0001, SQUARE_SHAPE);
			}
			p.x = -1.0 + width / 2 + width * (parameters[0].value-1);
			p.y = -1.0;
			p.z = 0.0;
			pushNewSubplan(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 2.0, width-0.0001, SQUARE_SHAPE);
			break;
		case PickupTemplate::VERTICAL:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameters[0].value, 1);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				pushNewSubplan(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 2.0, width-0.0001, SQUARE_SHAPE);
			}
			break;
		case PickupTemplate::VERTICAL_WITH_LIP_WRAP:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameters[0].value, 1);
			p.x = -1.0;
			p.y = 1.0 - width/2;
			pushNewSubplan(&newSubs, p, HORIZONTAL_PICKUP_CANE_ORIENTATION, 2.0, width, SQUARE_SHAPE);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				pushNewSubplan(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 2.0 - width, 
					width-0.0001, SQUARE_SHAPE);
			}
			break;
		case PickupTemplate::VERTICALS_AND_HORIZONTALS:
		{
			float verticals_width = 2.0 / MAX(parameters[0].value, 1);
			float horizontals_width = 1.0 / MAX(parameters[0].value/2, 1);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0 + verticals_width / 2 + verticals_width * i;
				p.y = 0.0;
				p.z = 0.0;
				pushNewSubplan(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 1.0, 
					verticals_width-0.0001, SQUARE_SHAPE);
				if (i % 2 == 1) {
					p.x = -1.0;
					p.y = -(horizontals_width / 2 + horizontals_width * (i-1) / 2);
					p.z = 0.0;
					pushNewSubplan(&newSubs, p, HORIZONTAL_PICKUP_CANE_ORIENTATION, 2.0, 
						horizontals_width-0.0001, SQUARE_SHAPE);
				}
			}
			break;
		}
		case PickupTemplate::VERTICAL_HORIZONTAL_VERTICAL:
		{
			float verticals_width = 2.0 / MAX(parameters[0].value, 1);
			float horizontals_width = 0.5 / MAX(parameters[0].value/3, 1);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0 + verticals_width / 2 + verticals_width * i;
				p.y = -1.0;
				p.z = 0.0;
				pushNewSubplan(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 0.75,
					verticals_width-0.0001, SQUARE_SHAPE);
				p.x = -1.0 + verticals_width / 2 + verticals_width * i;
				p.y = 0.25;
				p.z = 0.0;
				pushNewSubplan(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 0.75,
					verticals_width-0.0001, SQUARE_SHAPE);
				if (i % 3 == 2) {
					p.x = -1.0;
					p.y = 0.25 - horizontals_width / 2 - horizontals_width * (i-2) / 3;
					p.z = 0.0;
					pushNewSubplan(&newSubs, p, HORIZONTAL_PICKUP_CANE_ORIENTATION, 2.0, 
						horizontals_width-0.0001, SQUARE_SHAPE);
				}
			}
			break;
		}
	}

	subs = newSubs;
}


void PickupPlan :: setTemplateType(enum PickupTemplate::Type _type, bool force) {

	if (!force && type == _type)
		return;

	this->type = _type;

	parameters.clear();
	switch (type) {
		case PickupTemplate::VERTICALS_AND_HORIZONTALS:
			parameters.push_back(TemplateParameter(10, string("Column count"), 6, 30));
			break;
		case PickupTemplate::VERTICAL:
			parameters.push_back(TemplateParameter(10, string("Column count"), 6, 30));
			break;
		case PickupTemplate::MURRINE_COLUMN:
			parameters.push_back(TemplateParameter(10, string("Column count"), 6, 30));
			break;
		case PickupTemplate::MURRINE_ROW:
			parameters.push_back(TemplateParameter(10, string("Column count"), 6, 30));
			break;
		case PickupTemplate::MURRINE:
			parameters.push_back(TemplateParameter(6, string("Row/Column count"), 6, 14));
			parameters.push_back(TemplateParameter(10, string("Thickness"), 1, 40));
			break;
		case PickupTemplate::RETICELLO_VERTICAL_HORIZONTAL:
			parameters.push_back(TemplateParameter(10, string("Column count"), 6, 30));
			break;
		case PickupTemplate::VERTICAL_HORIZONTAL_VERTICAL:
			parameters.push_back(TemplateParameter(10, string("Column count"), 6, 20));
			break;
		case PickupTemplate::VERTICAL_WITH_LIP_WRAP:
			parameters.push_back(TemplateParameter(10, string("Column count"), 6, 30));
			break;
	}

	subs.clear(); // don't carry over any of the current stuff
	updateSubs();
}

enum PickupTemplate::Type PickupPlan :: getTemplateType() {

	return this->type;
}

unsigned int PickupPlan :: getParameterCount()
{
	return parameters.size();
}

void PickupPlan :: getParameter(unsigned int _index, TemplateParameter* dest)
{
	assert(_index < parameters.size());
	*dest = parameters[_index];
}

void PickupPlan :: setParameter(unsigned int _index, int _value)
{
	assert(_index < parameters.size());
	parameters[_index].value = _value;
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
