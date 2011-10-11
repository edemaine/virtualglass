

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
		void setPullTemplate(PullTemplate* pt, PullPlan* defaultSubplan);
		void setSubplan(unsigned int index, PullPlan* subplan);
		void setTwist(float twist);
		void setLength(float length);
		bool isBase();
		void setColor(float r, float g, float b, float a);
		Color getColor();
	
	private:
		// Variables
		PullTemplate* pullTemplate;
		vector<PullPlan*> subplans;
		float twist;
		float length;	
		Color color;
		bool isBasePullPlan;
};

#endif

