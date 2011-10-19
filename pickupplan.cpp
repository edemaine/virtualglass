
#include "pickupplan.h"

PickupPlan :: PickupPlan(int pickupTemplate)
{
	setTemplate(new PickupTemplate(pickupTemplate));
}

void PickupPlan :: setTemplate(PickupTemplate* pt)
{
	this->pickupTemplate = pt;
	this->subplans.clear();

	// initialize the pickup plan's subplans to be something boring and base
	Color color;
	color.r = color.g = color.b = 1.0;
	color.a = 0.4;
	for (unsigned int i = 0; i < pt->subpulls.size(); ++i)
	{
		subplans.push_back(new PullPlan(CIRCLE_BASE_TEMPLATE, true, color)); 
	}
}

PickupTemplate* PickupPlan :: getTemplate()
{
	return this->pickupTemplate;
}

void PickupPlan :: setSubplan(unsigned int index, PullPlan* plan)
{
	if (index < this->pickupTemplate->subpulls.size())
	{
		// If the subplan is base, then it is ok to throwaway, 
		// since base plans are only made as copies from some factory-type process
		if (subplans[index]->isBase)
			 delete subplans[index];
		subplans[index] = plan;
	}
}

vector<PullPlan*> PickupPlan :: getSubplans()
{
	return subplans;
}






