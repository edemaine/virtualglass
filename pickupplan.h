

#ifndef PICKUPPLAN_H
#define PICKUPPLAN_H

#include <stdlib.h>
#include <vector>
#include "pickuptemplate.h"
#include "pullplan.h"

using std::vector;

class PickupPlan
{
	public:
		PickupPlan(int pickupTemplate);
		PickupPlan();

		void setTemplateType(int templateType);
		int getTemplateType();

                void setParameter(int param, int newValue);
                int getParameter(int param);
                char* getParameterName(int param);

		PickupPlan* copy();

		vector<PullPlan*> subplans;
                vector<SubpickupTemplate*> subtemps;
		PullPlan* overlayColorPlan;
		PullPlan* underlayColorPlan;
		bool useUnderlay;

	private:
		void updateSubplans();
                void updateSubtemps();
		int templateType;	
                vector<int> parameterValues;
                vector<char*> parameterNames;
		PullPlan* defaultSubplan;
};

#endif

