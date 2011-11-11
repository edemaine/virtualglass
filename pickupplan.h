

#ifndef PICKUPPLAN_H
#define PICKUPPLAN_H

class PickupPlanLibraryWidget;

#include <stdlib.h>
#include <vector>
#include "pickuptemplate.h"
#include "pullplan.h"
#include "pickupplanlibrarywidget.h"

class PickupPlan;

class PickupPlan
{
	public:
		PickupPlan(int pickupTemplate);

		void setLibraryWidget(PickupPlanLibraryWidget* pkplw);
		PickupPlanLibraryWidget* getLibraryWidget();

		void setTemplate(PickupTemplate* pt);
		PickupTemplate* getTemplate();

		vector<PullPlan*> subplans;

	private:
		// Variables
		PickupTemplate* pickupTemplate;
		PickupPlanLibraryWidget* libraryWidget;
};

#endif

