
#include "pickupplan.h"

PickupPlan :: PickupPlan(int pickupTemplate)
{
	this->pickupTemplate = new PickupTemplate(pickupTemplate);
	for (unsigned int i = 0; i < this->pickupTemplate->subpulls.size(); ++i)
	{
		Color color;
		color.r = color.g = color.b = 1.0;
		color.a = 0.4;
		this->subplans.push_back(new PullPlan(ONE_COLORED_CASING_TEMPLATE, true, color));
	}
}

void PickupPlan :: setTemplate(PickupTemplate* newTemplate)
{
	vector<PullPlan*> newSubplans;

	// For each new subpull, see if its group exists in the current template
	for (unsigned int i = 0; i < newTemplate->subpulls.size(); ++i)
	{
		int group = newTemplate->subpulls[i]->group;
	
		// Look for the group in the old template, copy the plan if found
		bool matchFound = false;
		for (unsigned int j = 0; j < this->pickupTemplate->subpulls.size(); ++j)
		{
			if (group == this->pickupTemplate->subpulls[j]->group)
			{
				newSubplans.push_back(this->subplans[j]);
				matchFound = true;
				break;
			}
		}

		if (!matchFound)
		{
			Color color;
			color.r = color.g = color.b = 1.0;
			color.a = 0.4;
			newSubplans.push_back(new PullPlan(ONE_COLORED_CASING_TEMPLATE, true, color));
		}
	}

	this->pickupTemplate = newTemplate;
	this->subplans.clear();
	for (unsigned int i = 0; i < this->pickupTemplate->subpulls.size(); ++i)
	{
		subplans.push_back(newSubplans[i]);
	}
}

PickupTemplate* PickupPlan :: getTemplate()
{
	return this->pickupTemplate;
}







