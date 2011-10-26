
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
	
	for (unsigned int i = 0; i < pt->subpulls.size(); ++i)
	{
                // Set color based on group, only support for 3 unique groups;
                // Additional groups all show up grey
                Color color;
                color.r = color.g = color.b = 1.0;
                color.a = 0.4;
                switch (pt->subpulls[i].group)
                {
                        case 0:
                                color.r = color.g = 0.4;
                                break;
                        case 1:
                                color.r = color.b = 0.4;
                                break;
                        case 2:
                                color.g = color.b = 0.4;
                                break;
                        default:
                                break;
                }

		subplans.push_back(new PullPlan(CIRCLE_BASE_TEMPLATE, true, color)); 
	}
}

PickupTemplate* PickupPlan :: getTemplate()
{
	return this->pickupTemplate;
}







