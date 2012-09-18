
#include "pickupplan.h"

PickupPlan :: PickupPlan(enum PickupTemplate::Type _type) {

	defaultSubplan = new PullPlan(PullTemplate::baseCircle);
	defaultGlassColor = new GlassColor();
	casingGlassColor = underlayGlassColor = overlayGlassColor = defaultGlassColor;

	setTemplateType(_type, true);
}

PickupPlan* PickupPlan :: copy() const {
	
	PickupPlan* c = new PickupPlan(type);

	for (unsigned int i = 0; i < parameters.size(); ++i)
	{
		c->parameters[i].value = parameters[i].value;
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
    Point location, enum PickupCane::Orientation ori, float length, float width, int shape, int group) {

	if (newSubs->size() < subs.size())
	{
		newSubs->push_back(SubpickupTemplate(subs[newSubs->size()].plan,
			location, ori, length, width, shape, group));
	}
	else // you've run out of existing subplans copy from
	{
		newSubs->push_back(SubpickupTemplate(defaultSubplan,
			location, ori, length, width, shape, group));
	}
}

void PickupPlan :: updateSubs() {

	vector<SubpickupTemplate> newSubs;

	Point p;
	float width;
	switch (this->type) {
		case PickupTemplate::murrine:
                        width = 2.0 / MAX(parameters[0].value, 1);
			for (int r = 0; r < parameters[0].value; ++r)
			{
				for (int c = 0; c < parameters[0].value; ++c)
				{
					p.x = -1.0 + width / 2 + width * r;
					p.y = -1.0 + width / 2 + width * c;
					p.z = -width/2;
					pushNewSubplan(&newSubs, p, PickupCane::murrine, parameters[1].value*0.005 + 0.005, width-0.0001,
						SQUARE_SHAPE, 1);
				}
			}
                        break;
		case PickupTemplate::murrineColumn:
			width = 2.0 / MAX(parameters[0].value, 1);
			for (int i = 0; i < parameters[0].value-1; ++i) {
				p.x = 1.0 - width / 2;
				p.y = -1.0 + width / 2 + width * (parameters[0].value - 1 - i);
				p.z = -width/2;
				pushNewSubplan(&newSubs, p, PickupCane::murrine, width, width-0.0001,
					SQUARE_SHAPE, 1);
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				p.z = 0.0;
				pushNewSubplan(&newSubs, p, PickupCane::vertical, 2.0, width-0.0001,
					CIRCLE_SHAPE, 0);
			}
			p.x = 1.0 - width / 2;
			p.y = -1.0 + width / 2;
			p.z = -width/2;
			pushNewSubplan(&newSubs, p, PickupCane::murrine, width, width-0.0001,
				SQUARE_SHAPE, 1);
			break;
		case PickupTemplate::murrineRow:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameters[0].value, 1);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				p.z = 0.0;
				pushNewSubplan(&newSubs, p, PickupCane::vertical, 1.0 - width/2, width-0.0001,
					CIRCLE_SHAPE, 0);
				p.y = 0.0;
				p.x = -1.0 + width / 2 + width * i;
				p.z = -width/2;
				pushNewSubplan(&newSubs, p, PickupCane::murrine, width, width-0.0001,
					SQUARE_SHAPE, 1);
				p.x = -1.0 + width / 2 + width * i;
				p.y = width/2;
				p.z = 0.0;
				pushNewSubplan(&newSubs, p, PickupCane::vertical, 1.0 - width/2, width-0.0001,
					CIRCLE_SHAPE, 2);
			}
			break;
		case PickupTemplate::reticelloVerticalHorizontal:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameters[0].value, 1);
			for (int i = 0; i < parameters[0].value-1; ++i) {
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				p.z = 0.0;
				pushNewSubplan(&newSubs, p, PickupCane::vertical, 2.0, width-0.0001,
					SQUARE_SHAPE, 0);
				p.x = -1.0;
				p.y = 1.0 - (1.5 * width + width * i);
				p.z = -width/2;
				pushNewSubplan(&newSubs, p, PickupCane::horizontal, 2.0, width-0.0001,
					SQUARE_SHAPE, 1);
			}
			p.x = -1.0 + width / 2 + width * (parameters[0].value-1);
			p.y = -1.0;
			p.z = 0.0;
			pushNewSubplan(&newSubs, p, PickupCane::vertical, 2.0, width-0.0001,
				SQUARE_SHAPE, 0);
			break;
		case PickupTemplate::verticals:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameters[0].value, 1);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				pushNewSubplan(&newSubs, p, PickupCane::vertical, 2.0, width-0.0001,
					SQUARE_SHAPE, 0);
			}
			break;
		case PickupTemplate::verticalWithLipWrap:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameters[0].value, 1);
			p.x = -1.0;
			p.y = 1.0 - width/2;
			pushNewSubplan(&newSubs, p, PickupCane::horizontal, 2.0, width, SQUARE_SHAPE, 0);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				pushNewSubplan(&newSubs, p, PickupCane::vertical, 2.0 - width, width-0.0001,
					SQUARE_SHAPE, 1);
			}
			break;
		case PickupTemplate::verticalsAndHorizontals:
		{
			float verticals_width = 2.0 / MAX(parameters[0].value, 1);
			float horizontals_width = 1.0 / MAX(parameters[0].value/2, 1);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0 + verticals_width / 2 + verticals_width * i;
				p.y = 0.0;
				p.z = 0.0;
				pushNewSubplan(&newSubs, p, PickupCane::vertical, 1.0, verticals_width-0.0001,
					SQUARE_SHAPE, 0);
				if (i % 2 == 1) {
					p.x = -1.0;
					p.y = -(horizontals_width / 2 + horizontals_width * (i-1) / 2);
					p.z = 0.0;
					pushNewSubplan(&newSubs, p, PickupCane::horizontal, 2.0,
					horizontals_width-0.0001, SQUARE_SHAPE, 1);
				}
			}
			break;
		}
		case PickupTemplate::verticalHorizontalVertical:
		{
			float verticals_width = 2.0 / MAX(parameters[0].value, 1);
			float horizontals_width = 0.5 / MAX(parameters[0].value/3, 1);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0 + verticals_width / 2 + verticals_width * i;
				p.y = -1.0;
				p.z = 0.0;
				pushNewSubplan(&newSubs, p, PickupCane::vertical, 0.75,
					verticals_width-0.0001, SQUARE_SHAPE, 0);
				p.x = -1.0 + verticals_width / 2 + verticals_width * i;
				p.y = 0.25;
				p.z = 0.0;
				pushNewSubplan(&newSubs, p, PickupCane::vertical, 0.75,
					verticals_width-0.0001, SQUARE_SHAPE, 2);
				if (i % 3 == 2) {
					p.x = -1.0;
					p.y = 0.25 - horizontals_width / 2 - horizontals_width * (i-2) / 3;
					p.z = 0.0;
					pushNewSubplan(&newSubs, p, PickupCane::horizontal, 2.0, horizontals_width-0.0001,
					SQUARE_SHAPE, 1);
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
		case PickupTemplate::verticalsAndHorizontals:
			parameters.push_back(TemplateParameter(14, string("Column count")));
			break;
		case PickupTemplate::verticals:
			parameters.push_back(TemplateParameter(14, string("Column count")));
			break;
		case PickupTemplate::murrineColumn:
			parameters.push_back(TemplateParameter(14, string("Column count")));
			break;
		case PickupTemplate::murrineRow:
			parameters.push_back(TemplateParameter(14, string("Column count")));
			break;
		case PickupTemplate::murrine:
			parameters.push_back(TemplateParameter(10, string("Row/Column count")));
			parameters.push_back(TemplateParameter(10, string("Thickness")));
			break;
		case PickupTemplate::reticelloVerticalHorizontal:
			parameters.push_back(TemplateParameter(14, string("Column count")));
			break;
		case PickupTemplate::verticalHorizontalVertical:
			parameters.push_back(TemplateParameter(14, string("Column count")));
			break;
		case PickupTemplate::verticalWithLipWrap:
			parameters.push_back(TemplateParameter(14, string("Column count")));
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
	dest->value = parameters[_index].value;
	dest->name = parameters[_index].name;
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
