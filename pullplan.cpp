
#include "pullplan.h"

PullPlan :: PullPlan(PullTemplate* pt, vector<PullPlan> subplans, float twist, float length)
{
	this->pullTemplate = pt;

	this->subplans.clear();
	for (unsigned int i = 0; i < subplans.size(); ++i)
	{	
		this->subplans.push_back(subplans[i]);
	}

	this->twist = twist;
	this->length = length;
}

