

#ifndef PICKUPPLAN_H
#define PICKUPPLAN_H

#include <cstdio>
#include <stdlib.h>
#include <string>
#include <vector>
#include "templateparameter.h"
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

                unsigned int getParameterCount();
                void getParameter(unsigned int index, TemplateParameter* dest);
                void setParameter(unsigned int index, int value);

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
		vector<TemplateParameter> parameters;
		PullPlan* defaultSubplan;
		GlassColor* defaultGlassColor;
	
};

PickupPlan *deep_copy(const PickupPlan *);
void deep_delete(PickupPlan *);

#endif

