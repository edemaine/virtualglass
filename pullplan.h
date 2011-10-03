

#ifndef PULLPLAN_H
#define PULLPLAN_H

#include <stdlib.h>
#include <vector>
#include "pulltemplate.h"

class PullPlan;

class PullPlan
{
	public:
		PullPlan(PullTemplate* pt, vector<PullPlan> subplans, float twist, float length);
	
	private:
		PullTemplate* pullTemplate;
		vector<PullPlan> subplans;
		float twist;
		float length;	

};

#endif

