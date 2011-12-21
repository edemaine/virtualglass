

#ifndef PULLPLAN_H
#define PULLPLAN_H

class PullPlanLibraryWidget;

#include <stdlib.h>
#include <vector>
#include "pulltemplate.h"

class PullPlan;

class PullPlan
{
	public:
		PullPlan();
		PullPlan(int pullTemplate, Color* color);

		void setTemplate(PullTemplate* pt);
		PullTemplate* getTemplate();

		PullPlan* copy();

		vector<PullPlan*> subplans;
		float twist;
		Color* color;
		bool hasDependencyOn(Color* color);
		bool hasDependencyOn(PullPlan* pullPlan);

	private:
		// Variables
		PullTemplate* pullTemplate;
};

#endif

