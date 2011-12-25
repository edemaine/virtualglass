
#include "pullplan.h"


PullPlan :: PullPlan()
{
}

PullPlan :: PullPlan(int pullTemplate, Color* color)
{
	// initialize all the variables
	this->pullTemplate = new PullTemplate(pullTemplate);
	this->color = color;
	this->twist = 0.0;
}

PullPlan* PullPlan :: copy()
{
	PullPlan* c = new PullPlan();

	c->color = this->color;
	c->twist = this->twist;
	c->pullTemplate = this->pullTemplate->copy();		
	c->subplans.clear();
	for (unsigned int i = 0; i < this->subplans.size(); ++i)
	{
		c->subplans.push_back(this->subplans[i]);
	}

	return c;
}

bool PullPlan :: hasDependencyOn(PullPlan* plan)
{
	if (this == plan)
		return true;
	if (this->pullTemplate->isBase())
		return false;

        bool childrenAreDependent = false;
        for (unsigned int i = 0; i < subplans.size(); ++i)
        {
                if (subplans[i]->hasDependencyOn(plan))
                {
                        childrenAreDependent = true;
                        break;
                }
        }	

	return childrenAreDependent;
}

bool PullPlan :: hasDependencyOn(Color* color)
{
	if (this->color == color) 
		return true;
	if (this->pullTemplate->isBase())
		return false;

	bool childrenAreDependent = false;
	for (unsigned int i = 0; i < subplans.size(); ++i)
	{
		if (subplans[i]->hasDependencyOn(color))
		{
			childrenAreDependent = true;
			break;
		}
	}
	
	return childrenAreDependent;
}

/*
The key to setting the new template in an `intuitive way' is
to carry over properties of the old one, and create a new set of subplans
that is a natural mapping from the old subplans, even though
they may have different shape, number, etc.
*/
void PullPlan :: setTemplate(PullTemplate* newTemplate)
{
	// Find reference circular and square subcanes
        Color* color;
	color = new Color();
	color->r = color->g = color->b = 1.0;
	color->a = 0.0;
	PullPlan* circlePullPlan = NULL;
	PullPlan* squarePullPlan = NULL;

	for (unsigned int i = 0; i < MIN(pullTemplate->subtemps.size(), subplans.size()); ++i)
	{
		if (subplans[i]->getTemplate()->type == AMORPHOUS_BASE_PULL_TEMPLATE)
		{
			circlePullPlan = subplans[i];
			squarePullPlan = subplans[i];
		}
		else if (this->pullTemplate->subtemps[i].shape == CIRCLE_SHAPE
			&& (circlePullPlan == NULL || circlePullPlan->color->a < 0.0001))
			circlePullPlan = this->subplans[i];
		else if (this->pullTemplate->subtemps[i].shape == SQUARE_SHAPE
			&& (squarePullPlan == NULL || squarePullPlan->color->a < 0.0001))
			squarePullPlan = this->subplans[i];
	}

	if (circlePullPlan == NULL)
		circlePullPlan = new PullPlan(CIRCLE_BASE_PULL_TEMPLATE, color);
	if (squarePullPlan == NULL)
		squarePullPlan = new PullPlan(SQUARE_BASE_PULL_TEMPLATE, color);

	// create the new subplans based on template
	newTemplate->setCasingThickness(this->pullTemplate->getCasingThickness());
	this->pullTemplate = newTemplate;
	this->subplans.clear();
	for (unsigned int i = 0; i < newTemplate->subtemps.size(); ++i)
	{
		switch (newTemplate->subtemps[i].shape)
		{
			case CIRCLE_SHAPE:
				subplans.push_back(circlePullPlan);
				break;
			case SQUARE_SHAPE:
				subplans.push_back(squarePullPlan);
				break;
		}
	}
}

PullTemplate* PullPlan :: getTemplate()
{
	return this->pullTemplate;
}






