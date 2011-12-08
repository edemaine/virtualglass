
#include "pickupplan.h"

PickupPlan :: PickupPlan(int pickupTemplate)
{
	setTemplate(new PickupTemplate(pickupTemplate));
}

PickupPlan* PickupPlan :: copy()
{
	PickupPlan* c = new PickupPlan(this->pickupTemplate->type);

	c->pickupTemplate = this->pickupTemplate->copy();
	c->subplans.clear();
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







