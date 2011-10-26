

#ifndef PICKUPPLAN_H
#define PICKUPPLAN_H

#include <stdlib.h>
#include <vector>
#include "pickuptemplate.h"
#include "pullplan.h"

class PickupPlan;

class PickupPlan
{
	public:
		PickupPlan(int pickupTemplate);

		void setTemplate(PickupTemplate* pt);
		PickupTemplate* getTemplate();

		vector<PullPlan*> subplans;
	
	private:
		// Variables
		PickupTemplate* pickupTemplate;
};

#endif

