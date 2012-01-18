
#include "pickupplan.h"


PickupPlan :: PickupPlan()
{
}

PickupPlan :: PickupPlan(int pickupTemplate)
{
	setTemplate(new PickupTemplate(pickupTemplate));
}

void PickupPlan :: updateSubplans()
{
	// This fixes the subplans to correspond to a change in the pickup template
	// Eventually, template changes should go through a function in the 
	// pickup plan to ensure the changes are made no matter what. So it goes.
	if (this->subplans.size() > this->pickupTemplate->subtemps.size())
	{
		while (this->subplans.size() > this->pickupTemplate->subtemps.size())
		{
			this->subplans.pop_back();
		}
		return;
	} 
	else if (this->subplans.size() < this->pickupTemplate->subtemps.size())
	{
		while (this->subplans.size() < this->pickupTemplate->subtemps.size())
		{
			this->subplans.push_back(this->subplans.back());
		}
		return;
	}
}

PickupPlan* PickupPlan :: copy()
{
	PickupPlan* c = new PickupPlan();

	c->pickupTemplate = this->pickupTemplate->copy();
	for (unsigned int i = 0; i < this->subplans.size(); ++i)
	{
		c->subplans.push_back(this->subplans[i]);
	}

	return c;
}

void PickupPlan :: setTemplate(PickupTemplate* newTemplate)
{
	PullPlan* representativeSubplan;

	if (this->subplans.size() == 0)
	{
		Color* color;
		color = new Color();
		color->r = color->g = color->b = 1.0;
		color->a = 0.0;
		representativeSubplan = new PullPlan(CIRCLE_BASE_PULL_TEMPLATE, color);
	}
	else
		representativeSubplan = this->subplans[0];
        this->pickupTemplate = newTemplate;
        this->subplans.clear();
        for (unsigned int i = 0; i < this->getTemplate()->subtemps.size(); ++i)
        {
		this->subplans.push_back(representativeSubplan);			
        }
}

PickupTemplate* PickupPlan :: getTemplate()
{
	return this->pickupTemplate;
}







