

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
		void setLength(float length);

		float getTwist();
		float getLength();

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

