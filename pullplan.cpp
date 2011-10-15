
#include "pullplan.h"

PullPlan :: PullPlan()
{
	clear();	
}

void PullPlan :: clear()
{
	this->pullTemplate = NULL;
	this->subplans.clear();
	this->twist = 0.0;
	this->length = 1.0;
	isBasePullPlan = true;
	color.r = color.g = color.b = color.a = 1.0;
}

void PullPlan :: setTemplate(PullTemplate* pt, PullPlan* defaultSubplan)
{
	this->pullTemplate = pt;
	this->subplans.clear();
	this->subplans.reserve(pt->locations.size());
	for (unsigned int i = 0; i < pt->locations.size(); ++i)
		subplans[i] = defaultSubplan;
	isBasePullPlan = false;
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
	isBasePullPlan = true;
}

void PullPlan :: setSubplan(unsigned int index, PullPlan* plan)
{
	if (index < pullTemplate->locations.size()) 
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

bool PullPlan :: isBase()
{
	return isBasePullPlan;
}

void PullPlan :: setTwist(float twist)
{
	this->twist = twist;
}

float PullPlan :: getTwist()
{
	return this->twist;
}

void PullPlan :: setLength(float length)
{
	this->length = length;
}

float PullPlan :: getLength()
{
	return this->length;
}



