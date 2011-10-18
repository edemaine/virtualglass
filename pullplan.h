

#ifndef PULLPLAN_H
#define PULLPLAN_H

#include <stdlib.h>
#include <vector>
#include "pulltemplate.h"

class PullPlan;

class PullPlan
{
	public:
		PullPlan();
		void clear();

		void setTemplate(PullTemplate* pt, PullPlan* defaultSubplan);
		PullTemplate* getTemplate();

		void setSubplan(unsigned int index, PullPlan* subplan);
		vector<PullPlan*> getSubplans();

		void setTwist(float twist);

		float getTwist();

		bool isBase;

		void setColor(int r, int g, int b, int a);
		Color getColor();
	
	private:
		// Variables
		PullTemplate* pullTemplate;
		vector<PullPlan*> subplans;
		float twist;
		Color color;
};

#endif

