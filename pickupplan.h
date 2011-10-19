

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

		void setSubplan(unsigned int index, PullPlan* subplan);
		vector<PullPlan*> getSubplans();
	
	private:
		// Variables
		PickupTemplate* pickupTemplate;
		vector<PullPlan*> subplans;
};

#endif

