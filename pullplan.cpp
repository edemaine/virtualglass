
#include "pullplan.h"

PullPlan :: PullPlan()
{
	clear();	
	isBase = true;
}

void PullPlan :: clear()
{
	this->pullTemplate = NULL;
	this->subplans.clear();
	this->twist = 0.0;
	color.r = color.g = color.b = color.a = 1.0;
}

void PullPlan :: setTemplate(PullTemplate* pt, PullPlan* defaultSubplan)
{
	this->pullTemplate = pt;
	this->subplans.clear();
	for (unsigned int i = 0; i < pt->subpulls.size(); ++i)
		subplans.push_back(defaultSubplan);
}

PullTemplate* PullPlan :: getTemplate()
{
	return this->pullTemplate;
}

void PullPlan :: setColor(float r, float g, float b, float a)
{
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
}

void PullPlan :: setSubplan(unsigned int index, PullPlan* plan)
{
	if (index < pullTemplate->subpulls.size())
		subplans[index] = plan;
}

vector<PullPlan*> PullPlan :: getSubplans()
{
	return subplans;
}

Color PullPlan :: getColor()
{
	return color;
}

void PullPlan :: setTwist(float twist)
{
	this->twist = twist;
}

float PullPlan :: getTwist()
{
	return this->twist;
}

