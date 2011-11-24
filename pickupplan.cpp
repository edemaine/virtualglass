
#include "pickupplan.h"

PickupPlan :: PickupPlan(int pickupTemplate)
{
	this->pickupTemplate = new PickupTemplate(pickupTemplate);
	this->libraryWidget = NULL;
	for (unsigned int i = 0; i < this->pickupTemplate->subpulls.size(); ++i)
	{
		Color* color = new Color();
		color->r = color->g = color->b = 1.0;
		color->a = 0.4;
		this->subplans.push_back(new PullPlan(CIRCLE_BASE_TEMPLATE, true, color));
	}
}

void PickupPlan :: setLibraryWidget(PickupPlanLibraryWidget* pkplw)
{
	this->libraryWidget = pkplw;
}

PickupPlanLibraryWidget* PickupPlan :: getLibraryWidget()
{
	return this->libraryWidget;
}

void PickupPlan :: setTemplate(PickupTemplate* newTemplate)
{
        // load these into the pull plan
        this->pickupTemplate = newTemplate;
        this->subplans.clear();
        for (unsigned int i = 0; i < this->getTemplate()->subpulls.size(); ++i)
        {
                Color* color = new Color();
                color->r = color->g = color->b = 1.0;
                color->a = 0.4;
                subplans.push_back(new PullPlan(CIRCLE_BASE_TEMPLATE, true, color));
        }
}

PickupTemplate* PickupPlan :: getTemplate()
{
	return this->pickupTemplate;
}







