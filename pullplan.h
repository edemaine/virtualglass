

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

		vector<PullPlan*> subplans;
		float twist;
		bool isBase;
		Color color;
	
	private:
		// Variables
		PullTemplate* pullTemplate;
};

#endif

