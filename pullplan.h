

#ifndef PULLPLAN_H
#define PULLPLAN_H

#include <stdlib.h>
#include <vector>
#include "pulltemplate.h"

class PullPlan;

class PullPlan
{
	public:
		PullPlan(int pullTemplate, bool isBase, Color color);

		void setTemplate(PullTemplate* pt);
		PullTemplate* getTemplate();

		void setSubplan(unsigned int index, PullPlan* subplan);
		vector<PullPlan*> getSubplans();

		float twist;
		bool isBase;
		Color color;
	
	private:
		// Variables
		PullTemplate* pullTemplate;
		vector<PullPlan*> subplans;
};

#endif

