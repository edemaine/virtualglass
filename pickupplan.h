

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

		PickupPlan* copy() const;

		vector<SubpickupTemplate> subs;
		Color* overlayColor;
		Color* underlayColor;

	private:
		void updateSubs();
		void pushNewSubplan(vector<SubpickupTemplate>* newSubs, Point location,
			int orientation, float length, float width, int shape, int group); 
		int templateType;	
                vector<int> parameterValues;
                vector<char*> parameterNames;
		PullPlan* defaultSubplan;
};

PickupPlan *deep_copy(const PickupPlan *);
void deep_delete(PickupPlan *);

#endif

