
#include "pullplan.h"

PullPlan :: PullPlan(int pullTemplate, bool isBase, Color color)
{
	setTemplate(new PullTemplate(pullTemplate));
	this->color = color;
	this->isBase = isBase;
	this->twist = 0.0;
}

void PullPlan :: setTemplate(PullTemplate* pt)
{
	this->pullTemplate = pt;
	this->subplans.clear();

	// initialize the pull plan's subplans to be something boring and base
	for (unsigned int i = 0; i < pt->subpulls.size(); ++i)
	{
		Color color;
		color.r = color.g = color.b = 1.0;
		color.a = 0.4;
		switch (pt->subpulls[i].shape)	
		{
			// this is a memory leak
			case CIRCLE_SHAPE:
				subplans.push_back(new PullPlan(CIRCLE_BASE_TEMPLATE, true, color)); 
				break;
			case SQUARE_SHAPE:
				subplans.push_back(new PullPlan(SQUARE_BASE_TEMPLATE, true, color)); 
				break;
		}
	}
}

PullTemplate* PullPlan :: getTemplate()
{
	return this->pullTemplate;
}

void PullPlan :: setSubplan(unsigned int index, PullPlan* plan)
{
	if (index < pullTemplate->subpulls.size())
	{
		// If the subplan is base, then it is ok to throwaway, 
		// since base plans are only made as copies from some factory-type process
		if (subplans[index]->isBase)
			 delete subplans[index];
		subplans[index] = plan;
	}
}

vector<PullPlan*> PullPlan :: getSubplans()
{
	return subplans;
}






