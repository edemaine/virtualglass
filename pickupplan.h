

#ifndef PICKUPPLAN_H
#define PICKUPPLAN_H

#include <stdlib.h>
#include <vector>
#include "pickuptemplate.h"
#include "pullplan.h"

class PickupPlan
{
	public:
		PickupPlan(int pickupTemplate);

		void setTemplate(PickupTemplate* pt);
		PickupTemplate* getTemplate();

		PickupPlan* copy();

		vector<PullPlan*> subplans;

	private:
		// Variables
		PickupTemplate* pickupTemplate;
};

#endif

