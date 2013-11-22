
#include "constants.h"
#include "templateparameter.h"
#include "subpickuptemplate.h"
#include "canetemplate.h"
#include "globalglass.h"
#include "cane.h"
#include "glasscolor.h"
#include "pickup.h"
#include "pieceeditorwidget.h"

Pickup :: Pickup(enum PickupTemplate::Type _type) 
{
	this->casingGlassColor_ = GlobalGlass::color();
	this->underlayGlassColor_ = GlobalGlass::color();
	this->overlayGlassColor_ = GlobalGlass::color();
	setTemplateType(_type, true);
}

Pickup* Pickup :: copy() const 
{
	Pickup* p = new Pickup(this->type_);

	p->overlayGlassColor_ = this->overlayGlassColor_;
	p->underlayGlassColor_ = this->underlayGlassColor_;
	p->casingGlassColor_ = this->casingGlassColor_;
	p->subs_ = this->subs_;
	p->parameters_ = this->parameters_;
	p->type_ = this->type_;

	return p;
}

void Pickup :: set(Pickup* p)
{
	this->overlayGlassColor_ = p->overlayGlassColor_;
	this->underlayGlassColor_ = p->underlayGlassColor_;
	this->casingGlassColor_ = p->casingGlassColor_;
	this->subs_ = p->subs_;
	this->parameters_ = p->parameters_;
	this->type_ = p->type_;
}

SubpickupTemplate Pickup::getSubpickupTemplate(unsigned int index)
{
	return this->subs_[index];
}

void Pickup::setSubpickupTemplate(SubpickupTemplate t, unsigned int index)
{
	this->subs_[index] = t;
}

unsigned int Pickup::subpickupCount()
{
	return this->subs_.size();
}

GlassColor* Pickup::overlayGlassColor()
{
	return this->overlayGlassColor_;
}

GlassColor* Pickup::underlayGlassColor()
{
	return this->underlayGlassColor_;
}

GlassColor* Pickup::casingGlassColor()
{
	return this->casingGlassColor_;
}

void Pickup::setOverlayGlassColor(GlassColor* c)
{
	this->overlayGlassColor_ = c;
}

void Pickup::setUnderlayGlassColor(GlassColor* c)
{
	this->underlayGlassColor_ = c;
}

void Pickup::setCasingGlassColor(GlassColor* c)
{
	this->casingGlassColor_ = c;
}

void Pickup :: pushNewSubcane(vector<SubpickupTemplate>* newSubs,
	Point3D location, enum PickupCaneOrientation ori, float length, float width, enum GeometricShape shape) 
{
	if (newSubs->size() < this->subs_.size())
	{
		newSubs->push_back(SubpickupTemplate(this->subs_[newSubs->size()].cane,
			location, ori, length, width, shape));
	}
	else // you've run out of existing subcanes copy from
	{
		newSubs->push_back(SubpickupTemplate(GlobalGlass::circleCane(),
			location, ori, length, width, shape));
	}
}

void Pickup :: updateSubs() 
{
	vector<SubpickupTemplate> newSubs;

	vector<TemplateParameter> &parameters = this->parameters_;

	Point3D p;
	float width, length;
	switch (this->type_) 
	{
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
					pushNewSubcane(&newSubs, p, MURRINE_PICKUP_CANE_ORIENTATION, 
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
				pushNewSubcane(&newSubs, p, MURRINE_PICKUP_CANE_ORIENTATION, width, width-0.0001, SQUARE_SHAPE);
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				p.z = 0.0;
				pushNewSubcane(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 2.0, width-0.0001, CIRCLE_SHAPE);
			}
			p.x = 1.0 - width / 2;
			p.y = -1.0 + width / 2;
			p.z = -width/2;
			pushNewSubcane(&newSubs, p, MURRINE_PICKUP_CANE_ORIENTATION, width, width-0.0001, SQUARE_SHAPE);
			break;
		case PickupTemplate::MURRINE_ROW:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameters[0].value, 1);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				p.z = 0.0;
				pushNewSubcane(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 1.0 - width/2, 
					width-0.0001, CIRCLE_SHAPE);
				p.y = 0.0;
				p.x = -1.0 + width / 2 + width * i;
				p.z = -width/2;
				pushNewSubcane(&newSubs, p, MURRINE_PICKUP_CANE_ORIENTATION, width, 
					width-0.0001, SQUARE_SHAPE);
				p.x = -1.0 + width / 2 + width * i;
				p.y = width/2;
				p.z = 0.0;
				pushNewSubcane(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 1.0 - width/2, 
					width-0.0001, CIRCLE_SHAPE);
			}
			break;
		case PickupTemplate::RETICELLO_VERTICAL_HORIZONTAL:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameters[0].value, 1);
			for (int i = 0; i < parameters[0].value/2; ++i)
			{
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				pushNewSubcane(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 1.0, width-0.0001, SQUARE_SHAPE);
				p.y = 0.0;
				pushNewSubcane(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 1.0, width-0.0001, SQUARE_SHAPE);
			}
			for (int i = 0; i < parameters[0].value; ++i)
			{
				p.x = 0.0 - width / 2 * (parameters[0].value % 2);
				p.y = -1.0 + width / 2 + width * i;                                
				pushNewSubcane(&newSubs, p, HORIZONTAL_PICKUP_CANE_ORIENTATION, 
					1.0 + width / 2 * (parameters[0].value % 2), width-0.0001, SQUARE_SHAPE);
			}
			break;
		case PickupTemplate::VERTICAL:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameters[0].value, 1);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				//p.z = 2.0;
				pushNewSubcane(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 2.0, width-0.0001, SQUARE_SHAPE);
			}
			break;
		case PickupTemplate::VERTICAL_WITH_LIP_WRAP:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameters[0].value, 1);
			p.x = -1.0;
			p.y = 1.0 - width/2;
			pushNewSubcane(&newSubs, p, HORIZONTAL_PICKUP_CANE_ORIENTATION, 2.0, width, SQUARE_SHAPE);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				pushNewSubcane(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 2.0 - width, 
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
				pushNewSubcane(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 1.0, 
					verticals_width-0.0001, SQUARE_SHAPE);
				if (i % 2 == 1) {
					p.x = -1.0;
					p.y = -(horizontals_width / 2 + horizontals_width * (i-1) / 2);
					p.z = 0.0;
					pushNewSubcane(&newSubs, p, HORIZONTAL_PICKUP_CANE_ORIENTATION, 2.0, 
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
				pushNewSubcane(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 0.75,
					verticals_width-0.0001, SQUARE_SHAPE);
				p.x = -1.0 + verticals_width / 2 + verticals_width * i;
				p.y = 0.25;
				p.z = 0.0;
				pushNewSubcane(&newSubs, p, VERTICAL_PICKUP_CANE_ORIENTATION, 0.75,
					verticals_width-0.0001, SQUARE_SHAPE);
				if (i % 3 == 2) {
					p.x = -1.0;
					p.y = 0.25 - horizontals_width / 2 - horizontals_width * (i-2) / 3;
					p.z = 0.0;
					pushNewSubcane(&newSubs, p, HORIZONTAL_PICKUP_CANE_ORIENTATION, 2.0, 
						horizontals_width-0.0001, SQUARE_SHAPE);
				}
			}
			break;
		}
	}

	this->subs_ = newSubs;
}


void Pickup :: setTemplateType(enum PickupTemplate::Type _type, bool force) 
{
	if (!force && this->type_ == _type)
		return;

	vector<TemplateParameter> &parameters = this->parameters_;
	
	this->type_ = _type;
	parameters.clear();
	switch (this->type_) 
	{
		case PickupTemplate::VERTICALS_AND_HORIZONTALS:
			parameters.push_back(TemplateParameter(10, string("Column count:"), 6, 30));
			break;
		case PickupTemplate::VERTICAL:
			parameters.push_back(TemplateParameter(10, string("Column count:"), 6, 30));
			break;
		case PickupTemplate::MURRINE_COLUMN:
			parameters.push_back(TemplateParameter(10, string("Column count:"), 6, 30));
			break;
		case PickupTemplate::MURRINE_ROW:
			parameters.push_back(TemplateParameter(10, string("Column count:"), 6, 30));
			break;
		case PickupTemplate::MURRINE:
			parameters.push_back(TemplateParameter(6, string("Row/Column count:"), 6, 14));
			parameters.push_back(TemplateParameter(10, string("Thickness:"), 1, 40));
			break;
		case PickupTemplate::RETICELLO_VERTICAL_HORIZONTAL:
			parameters.push_back(TemplateParameter(10, string("Column count:"), 6, 30));
			break;
		case PickupTemplate::VERTICAL_HORIZONTAL_VERTICAL:
			parameters.push_back(TemplateParameter(10, string("Column count:"), 6, 20));
			break;
		case PickupTemplate::VERTICAL_WITH_LIP_WRAP:
			parameters.push_back(TemplateParameter(10, string("Column count:"), 6, 30));
			break;
	}

	this->subs_.clear(); // don't carry over any of the current stuff
	updateSubs();
}

enum PickupTemplate::Type Pickup :: templateType() 
{
	return this->type_;
}

unsigned int Pickup :: parameterCount()
{
	return this->parameters_.size();
}

void Pickup :: getParameter(unsigned int _index, TemplateParameter* dest)
{
	assert(_index < this->parameters_.size());
	*dest = this->parameters_[_index];
}

void Pickup :: setParameter(unsigned int _index, int _value)
{
	assert(_index < this->parameters_.size());
	this->parameters_[_index].value = _value;
	updateSubs();
}

Pickup *deep_copy(const Pickup *_pickup) 
{
	assert(_pickup);
	Pickup *pickup = _pickup->copy();
	for (unsigned int i = 0; i < pickup->subpickupCount(); ++i)
	{
		SubpickupTemplate t = pickup->getSubpickupTemplate(i);
		t.cane = deep_copy(t.cane);
		pickup->setSubpickupTemplate(t, i);
	}
	return pickup;
}

void deep_delete(Pickup *pickup)
{
	assert(pickup);
	for (unsigned int i = 0; i < pickup->subpickupCount(); ++i)
	{
		SubpickupTemplate t = pickup->getSubpickupTemplate(i);
		delete t.cane;
		t.cane = NULL;
		pickup->setSubpickupTemplate(t, i);
	}
	delete pickup;
}
