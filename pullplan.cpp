
#include "pullplan.h"


PullPlan :: PullPlan(int templateType, Color* color)
{
	defaultCircleSubplan = defaultSquareSubplan = NULL;
	this->color = color;
	this->twist = 0.0;
        this->casingThickness = 0.1;
        this->templateType = -1; // to guarantee setTemplateType goes through
	setTemplateType(templateType);
	activated = new vector<bool>(this->subs.size(),false);
}

PullPlan* PullPlan :: copy()
{
	PullPlan* c = new PullPlan(this->templateType, this->color);
	c->shape = this->shape;
	c->casingThickness = this->casingThickness;
	c->twist = this->twist;
	c->color = this->color;
	for (unsigned int i = 0; i < this->activated->size(); ++i)
	{
		c->activated[i] = this->activated[i];
	}

	for (unsigned int i = 0; i < this->parameterNames.size(); ++i)
	{
		c->parameterValues[i] = this->parameterValues[i];
	}	
	c->updateSubs(); 

	for (unsigned int i = 0; i < this->subs.size(); ++i)
	{
		c->subs[i].plan = this->subs[i].plan;
	}

	return c;
}

bool PullPlan :: hasDependencyOn(PullPlan* plan)
{
	if (this == plan)
		return true;
	if (this->isBase())
		return false;

        bool childrenAreDependent = false;
        for (unsigned int i = 0; i < subs.size(); ++i)
        {
                if (subs[i].plan->hasDependencyOn(plan))
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
	if (this->isBase())
		return false;

	bool childrenAreDependent = false;
	for (unsigned int i = 0; i < subs.size(); ++i)
	{
		if (subs[i].plan->hasDependencyOn(color))
		{
			childrenAreDependent = true;
			break;
		}
	}
	
	return childrenAreDependent;
}

bool PullPlan :: isBase()
{
	return (this->templateType == CIRCLE_BASE_PULL_TEMPLATE
		|| this->templateType == SQUARE_BASE_PULL_TEMPLATE
		|| this->templateType == AMORPHOUS_BASE_PULL_TEMPLATE);
}

void PullPlan :: setTemplateType(int templateType)
{
	if (templateType == this->templateType)
		return;

	this->templateType = templateType;

        // If the pull template has subplans and you
        // haven't initialized your default subplans yet, do it
        if (!isBase() && defaultCircleSubplan == NULL)
        {
                // initialize default subplans
                Color* defaultColor;
                defaultColor = new Color();
                defaultColor->r = defaultColor->g = defaultColor->b = 1.0;
                defaultColor->a = 0.0;
                defaultCircleSubplan = new PullPlan(CIRCLE_BASE_PULL_TEMPLATE, defaultColor);
                defaultSquareSubplan = new PullPlan(SQUARE_BASE_PULL_TEMPLATE, defaultColor);
        }

	parameterNames.clear();
	parameterValues.clear();
        char* tmp;
        switch (templateType)
        {
                case CIRCLE_BASE_PULL_TEMPLATE:
                        this->shape = CIRCLE_SHAPE;
                        break;
                case SQUARE_BASE_PULL_TEMPLATE:
                        shape = CIRCLE_SHAPE;
                        break;
                case AMORPHOUS_BASE_PULL_TEMPLATE:
                        shape = AMORPHOUS_SHAPE;
                        break;
                case CASED_CIRCLE_PULL_TEMPLATE:
                        shape = CIRCLE_SHAPE;
                        break;
                case CASED_SQUARE_PULL_TEMPLATE:
                        shape = CIRCLE_SHAPE;
                        break;
                case HORIZONTAL_LINE_CIRCLE_PULL_TEMPLATE:
                        shape = CIRCLE_SHAPE;
                        tmp = new char[100];
                        sprintf(tmp, "Row count:");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(3);
                        break;
                case HORIZONTAL_LINE_SQUARE_PULL_TEMPLATE:
                        shape = CIRCLE_SHAPE;
                        tmp = new char[100];
                        sprintf(tmp, "Row count:");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(3);
                        break;
                case SURROUNDING_CIRCLE_PULL_TEMPLATE:
                        shape = CIRCLE_SHAPE;
                        tmp = new char[100];
                        sprintf(tmp, "Count:");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(8);
                        break;
                case CROSS_PULL_TEMPLATE:
                        shape = CIRCLE_SHAPE;
                        tmp = new char[100];
                        sprintf(tmp, "Radial count:");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(3);
                        break;
                case SQUARE_OF_SQUARES_PULL_TEMPLATE:
                case SQUARE_OF_CIRCLES_PULL_TEMPLATE:
                        shape = CIRCLE_SHAPE;
                        tmp = new char[100];
                        sprintf(tmp, "Row count:");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(4);
                        break;
                case TRIPOD_PULL_TEMPLATE:
                        shape = CIRCLE_SHAPE;
                        tmp = new char[100];
                        sprintf(tmp, "Radial count:");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(3);
                        break;
                case SURROUNDING_SQUARE_PULL_TEMPLATE:
                        shape = CIRCLE_SHAPE;
                        tmp = new char[100];
                        sprintf(tmp, "Column count:");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(2);
                        break;
				case CUSTOM_CIRCLE_PULL_TEMPLATE:
						shape = CIRCLE_SHAPE;
						break;
				case CUSTOM_SQUARE_PULL_TEMPLATE:
						shape = SQUARE_SHAPE;
						break;
        }

	subs.clear(); // don't carry over any of the current stuff
	updateSubs();
}

void PullPlan :: setShape(int shape)
{
	this->shape = shape;
	updateSubs();
}

int PullPlan :: getShape()
{
	return this->shape;
}

void PullPlan :: setColor(Color* c)
{
	this->color = c;
}

Color* PullPlan :: getColor()
{
	return this->color;
}

void PullPlan :: setTwist(float t)
{
	this->twist = t;
}

float PullPlan :: getTwist()
{
	return this->twist;
}

int PullPlan :: getTemplateType()
{
	return this->templateType;
}

void PullPlan :: setParameter(int p, int v)
{
        parameterValues[p] = v;
        updateSubs();
}

int PullPlan :: getParameter(int p)
{
        return parameterValues[p];
}

char* PullPlan :: getParameterName(int p)
{
        return parameterNames[p];
}

unsigned int PullPlan :: getParameterCount()
{
	return this->parameterNames.size();
}

void PullPlan :: setCasingThickness(float t)
{
	this->casingThickness = t;
	updateSubs();
}

float PullPlan :: getCasingThickness()
{
	return this->casingThickness;	
}

void PullPlan :: activate(int i)
{
	if (i >= 0 && i < (int)activated->size())
		this->activated->at(i) = true;
}

void PullPlan :: deactivate(int i)
{
	if (i >= 0 && i < (int)activated->size())
		this->activated->at(i) = false;
}

bool PullPlan :: isActivated(int i)
{
	if (i >= 0 && i < (int)activated->size())
		return this->activated->at(i);
	return false;
}


void PullPlan :: pushNewSubpull(vector<SubpullTemplate>* newSubs, 
	int shape, Point p, float diameter, int group)
{
	if (newSubs->size() < subs.size())
	{
		newSubs->push_back(SubpullTemplate(subs[newSubs->size()].plan, CIRCLE_SHAPE, p, diameter, group));
	}
	else // you've run out of existing subplans copy from
	{
		switch (shape)
		{
			case CIRCLE_SHAPE:
				newSubs->push_back(SubpullTemplate(defaultCircleSubplan, CIRCLE_SHAPE, p, diameter, group));
				break;
			case SQUARE_SHAPE:
				newSubs->push_back(SubpullTemplate(defaultSquareSubplan, SQUARE_SHAPE, p, diameter, group));
				break;
		}
	}
//	Possible memory leak?
	activated = new vector<bool>(this->subs.size(),false);
}

void PullPlan :: updateSubs()
{
        Point p;
        float radius = 1.0 - casingThickness;

	vector<SubpullTemplate> newSubs;

        p.x = p.y = p.z = 0.0;
        switch (this->templateType)
        {
                case CASED_CIRCLE_PULL_TEMPLATE:
                        pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, radius * 2.0, 0);
                        break;
                case CASED_SQUARE_PULL_TEMPLATE:
                        if (this->shape == CIRCLE_SHAPE)
                        {
                                radius *= 1.0 / pow(2, 0.5);
                        }
                        pushNewSubpull(&newSubs, SQUARE_SHAPE, p, radius * 2.0, 0);
                        break;
                case HORIZONTAL_LINE_CIRCLE_PULL_TEMPLATE:
                {
                        int count = parameterValues[0];
                        for (int i = 0; i < count; ++i)
                        {
                                float littleRadius = (2 * radius / count) / 2;
                                p.x = -radius + littleRadius + i * 2 * littleRadius;
                                pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, littleRadius * 2.0, 0);
                        }
                        break;
                }
                case HORIZONTAL_LINE_SQUARE_PULL_TEMPLATE:
                {
                        if (this->shape == CIRCLE_SHAPE)
                                radius *= 0.9;

                        int count = parameterValues[0];
                        for (int i = 0; i < count; ++i)
                        {
                                float littleRadius = (2 * radius / count) / 2;
                                p.x = -radius + littleRadius + i * 2 * littleRadius;
                                pushNewSubpull(&newSubs, SQUARE_SHAPE, p, littleRadius * 2.0, 0);
                        }
                        break;
                }
                case SURROUNDING_CIRCLE_PULL_TEMPLATE:
                {
                        int count = parameterValues[0];
                        float theta = TWO_PI / count;
                        float k = sin(theta/2) / (1 + sin(theta/2));

                        p.x = p.y = 0.0;
                        pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, (1 - 2 * k) * 2 * radius, 0);
                        for (int i = 0; i < count; ++i)
                        {
                                p.x = (1.0 - k) * radius * cos(TWO_PI / count * i);
                                p.y = (1.0 - k) * radius * sin(TWO_PI / count * i);
                                pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, 2 * k * radius, 0);
                        }
                        break;
                }
                case CROSS_PULL_TEMPLATE:
                {
                        int count = parameterValues[0]-1;
			float littleRadius = (radius / (count + 0.5)) / 2.0;

			p.x = p.y = 0.0;
			pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, littleRadius * 2.0, 0);
                        for (int i = 0; i < count; ++i)
                        {
                                p.x = (i+1) * 2 * littleRadius;
                                p.y = 0.0;
                                pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, littleRadius * 2.0, 0);
                                p.x = 0.0;
                                p.y = (i+1) * 2 * littleRadius;
                                pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, littleRadius * 2.0, 0);
                                p.x = -((i+1) * 2 * littleRadius);
                                p.y = 0.0;
                                pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, littleRadius * 2.0, 0);
                                p.x = 0.0;
                                p.y = -((i+1) * 2 * littleRadius);
                                pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, littleRadius * 2.0, 0);
                        }
                        break;
                }
                case SQUARE_OF_CIRCLES_PULL_TEMPLATE:
                case SQUARE_OF_SQUARES_PULL_TEMPLATE:
                {
                        if (this->shape == CIRCLE_SHAPE)
                                radius *= 1 / SQRT_TWO;

                        int count = parameterValues[0];
                        float littleRadius = radius / count;

			// We add the subtemplates in this funny way so that the 
			// ith subcane is always at the same location regardless of
			// parameters. This is needed for delete to work correctly.
			for (int s = 0; s < count; ++s)
			{
				for (int i = 0; i < count; ++i)
				{
					for (int j = 0; j < count; ++j)
					{
						if (i > s || j > s)
							continue;
						p.x = -radius + littleRadius + 2 * littleRadius * i;
						p.y = -radius + littleRadius + 2 * littleRadius * j;
						if (this->templateType == SQUARE_OF_CIRCLES_PULL_TEMPLATE)
							pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, 2 * littleRadius, 0);
						else
							pushNewSubpull(&newSubs, SQUARE_SHAPE, p, 2 * littleRadius, 0);
					}
				}
			}
			break;
                }
                case TRIPOD_PULL_TEMPLATE:
                {
                        int count = parameterValues[0];
                        float littleRadius = radius / (2 * count - 1);

                        p.x = p.y = 0.0;
                        pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, 2 * littleRadius, 0);
			for (int i = 1; i < count; ++i)
			{
				for (int theta = 0; theta < 3; ++theta)
				{
                                        p.x = (littleRadius * 2 * i) * cos(TWO_PI / 3 * theta);
                                        p.y = (littleRadius * 2 * i) * sin(TWO_PI / 3 * theta);
                                        pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, littleRadius * 2, 0);
                                }
                        }
                        break;
                }
                case SURROUNDING_SQUARE_PULL_TEMPLATE:
                {
                        if (this->shape == CIRCLE_SHAPE)
                                radius *= 1 / SQRT_TWO;

                        int count = parameterValues[0];
                        float littleRadius = radius / (count + 2);

                        p.x = p.y = 0.0;
                        pushNewSubpull(&newSubs, SQUARE_SHAPE, p, 2 * littleRadius * count, 0);
                        for (int i = 0; i < count + 2; ++i)
                        {
				p.x = -2 * littleRadius * (count + 1) / 2.0 + 2 * littleRadius * i;
				p.y = -2 * littleRadius * (count + 1) / 2.0;
				pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, 2 * littleRadius, 1);
                        }
                        for (int j = 1; j < count + 2; ++j)
                        {
				p.x = -2 * littleRadius * (count + 1) / 2.0 + 2 * littleRadius * (count + 1);
				p.y = -2 * littleRadius * (count + 1) / 2.0 + 2 * littleRadius * j;
				pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, 2 * littleRadius, 1);
                        }
                        for (int i = count; i >= 0; --i)
                        {
				p.x = -2 * littleRadius * (count + 1) / 2.0 + 2 * littleRadius * i;
				p.y = -2 * littleRadius * (count + 1) / 2.0 + 2 * littleRadius * (count + 1);
				pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, 2 * littleRadius, 1);
                        }
                        for (int j = count; j >= 0; --j)
                        {
				p.x = -2 * littleRadius * (count+1) / 2.0;
				p.y = -2 * littleRadius * (count + 1) / 2.0 + 2 * littleRadius * j;
				pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, 2 * littleRadius, 1);
                        }
			

                        break;
                }
        }

	subs = newSubs;
//	Possible memory leak?
	activated = new vector<bool>(this->subs.size(),false);
}






