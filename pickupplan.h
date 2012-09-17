

#ifndef PICKUPPLAN_H
#define PICKUPPLAN_H

#include <cstdio>
#include <stdlib.h>
#include <vector>
#include "subpickuptemplate.h"
#include "pulltemplate.h"
#include "pickuptemplate.h"
#include "pullplan.h"
#include "glasscolor.h"

using std::vector;

class PickupPlan
{
	public:
		PickupPlan(enum PickupTemplate::Type type);
		PickupPlan();

		void setTemplateType(enum PickupTemplate::Type type, bool force=false);
		enum PickupTemplate::Type getTemplateType();

                void setParameter(int param, int newValue);
                int getParameter(int param);
                unsigned int getParameterCount();
                char* getParameterName(int param);

		PickupPlan* copy() const;

		vector<SubpickupTemplate> subs;
		GlassColor* overlayGlassColor;
		GlassColor* underlayGlassColor;
		GlassColor* casingGlassColor;

	private:
		void updateSubs();
		void pushNewSubplan(vector<SubpickupTemplate>* newSubs, Point location,
			enum PickupCane::Orientation ori, float length, float width, int shape, int group); 
		enum PickupTemplate::Type type;
                vector<int> parameterValues;
                vector<char*> parameterNames;
		PullPlan* defaultSubplan;
		GlassColor* defaultGlassColor;
	
};

PickupPlan *deep_copy(const PickupPlan *);
void deep_delete(PickupPlan *);

#endif

