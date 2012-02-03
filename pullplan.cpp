
#include "pullplan.h"
#include <cstdio>

PullPlan :: PullPlan(int templateType) {

	defaultColor = new Color;
	defaultColor->r = defaultColor->g = defaultColor->b = 1.0;
	defaultColor->a = 0.0;
	defaultCircleSubplan = defaultSquareSubplan = NULL;

	this->twist = 0.0;
	this->templateType = -1; // to guarantee setTemplateType goes through
	setTemplateType(templateType);
}

PullPlan* PullPlan :: copy() {

	PullPlan* c = new PullPlan(this->templateType);
	c->casings.clear();
	c->casings = this->casings;
	c->twist = this->twist;

	for (unsigned int i = 0; i < this->parameterNames.size(); ++i) {
		c->parameterValues[i] = this->parameterValues[i];
	}
	c->updateSubs();

	for (unsigned int i = 0; i < this->subs.size(); ++i) {
		c->subs[i].plan = this->subs[i].plan;
	}

	return c;
}

bool PullPlan :: hasDependencyOn(PullPlan* plan) {

	if (this == plan)
	return true;
	if (this->isBase())
	return false;

	bool childrenAreDependent = false;
	for (unsigned int i = 0; i < subs.size(); ++i) {
		if (subs[i].plan->hasDependencyOn(plan)) {
			childrenAreDependent = true;
			break;
		}
	}

	return childrenAreDependent;
}

bool PullPlan :: hasDependencyOn(Color* color) {

	for (unsigned int i = 0; i < casings.size(); ++i) {
		if (this->casings[0].color == color)
			return true;
	} 
	if (this->isBase())
		return false;

	bool childrenAreDependent = false;
	for (unsigned int i = 0; i < subs.size(); ++i) {
		if (subs[i].plan->hasDependencyOn(color)) {
			childrenAreDependent = true;
			break;
		}
	}

	return childrenAreDependent;
}

bool PullPlan :: isBase() {

	return (this->templateType == CIRCLE_BASE_PULL_TEMPLATE
		|| this->templateType == SQUARE_BASE_PULL_TEMPLATE
		|| this->templateType == AMORPHOUS_BASE_PULL_TEMPLATE);
}

void PullPlan :: setTemplateType(int templateType) {

	if (templateType == this->templateType)
		return;

	this->templateType = templateType;

        // If the pull template has subplans and you
        // haven't initialized your default subplans yet, do it
        if (!isBase() && defaultCircleSubplan == NULL) {
                // initialize default subplans
                defaultCircleSubplan = new PullPlan(CIRCLE_BASE_PULL_TEMPLATE);
                defaultSquareSubplan = new PullPlan(SQUARE_BASE_PULL_TEMPLATE);
        }

	parameterNames.clear();
	parameterValues.clear();
	char* tmp;
	casings.clear();
	casings.push_back(Casing(1.0, CIRCLE_SHAPE, defaultColor));
        switch (templateType) {
                case CIRCLE_BASE_PULL_TEMPLATE:
                        break;
                case SQUARE_BASE_PULL_TEMPLATE:
                        break;
                case AMORPHOUS_BASE_PULL_TEMPLATE:
                        casings[0].shape = AMORPHOUS_SHAPE;
                        break;
                case CASED_CIRCLE_PULL_TEMPLATE:
                        break;
                case CASED_SQUARE_PULL_TEMPLATE:
                        break;
                case HORIZONTAL_LINE_CIRCLE_PULL_TEMPLATE:
                        tmp = new char[100];
                        sprintf(tmp, "Row count:");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(3);
                        break;
                case HORIZONTAL_LINE_SQUARE_PULL_TEMPLATE:
                        tmp = new char[100];
                        sprintf(tmp, "Row count:");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(3);
                        break;
                case SURROUNDING_CIRCLE_PULL_TEMPLATE:
                        tmp = new char[100];
                        sprintf(tmp, "Count:");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(8);
                        break;
                case CROSS_PULL_TEMPLATE:
                        tmp = new char[100];
                        sprintf(tmp, "Radial count:");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(3);
                        break;
                case SQUARE_OF_SQUARES_PULL_TEMPLATE:
                case SQUARE_OF_CIRCLES_PULL_TEMPLATE:
                        tmp = new char[100];
                        sprintf(tmp, "Row count:");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(4);
                        break;
                case TRIPOD_PULL_TEMPLATE:
                        tmp = new char[100];
                        sprintf(tmp, "Radial count:");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(3);
                        break;
                case SURROUNDING_SQUARE_PULL_TEMPLATE:
                        tmp = new char[100];
                        sprintf(tmp, "Column count:");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(2);
                        break;
		case CUSTOM_CIRCLE_PULL_TEMPLATE:
			break;
		case CUSTOM_SQUARE_PULL_TEMPLATE:
			this->casings[0].shape = SQUARE_SHAPE;
			break;
        }

	subs.clear(); // don't carry over any of the current stuff
	updateSubs();
}

void PullPlan :: setCasingColor(Color* c, unsigned int index) {

	if (index >= this->casings.size())
		return;
	this->casings[index].color = c;
}

Color* PullPlan :: getOutermostCasingColor() {

	return this->casings[casings.size()-1].color;
}

Color* PullPlan :: getCasingColor(unsigned int index) {

	if (index >= this->casings.size())
		return NULL;
	return this->casings[index].color;
}

unsigned int PullPlan :: getCasingCount() {

	return this->casings.size();
}

void PullPlan :: setTwist(float t) {

	this->twist = t;
}

float PullPlan :: getTwist() {

	return this->twist;
}

int PullPlan :: getTemplateType() {

	return this->templateType;
}

void PullPlan :: setParameter(int p, int v) {

        parameterValues[p] = v;
        updateSubs();
}

int PullPlan :: getParameter(int p) {

        return parameterValues[p];
}

char* PullPlan :: getParameterName(int p) {

        return parameterNames[p];
}

unsigned int PullPlan :: getParameterCount() {

	return this->parameterNames.size();
}

void PullPlan :: removeCasing() {

	int count = casings.size();
	if (count < 2)
		return;

	float diff = casings[count-1].thickness - casings[count-2].thickness;
	casings.pop_back();
	for (unsigned int i = 0; i < casings.size(); ++i) {
		casings[i].thickness += diff;
	}
}

void PullPlan :: addCasing(int shape, Color* color) {
	
	for (unsigned int i = 0; i < casings.size(); ++i) {
		casings[i].thickness -= 0.1;
	}
	casings.push_back(Casing(1.0, shape, color));
}

void PullPlan :: setCasingThickness(float t, unsigned int index) {

	if (index >= this->casings.size())
		return;
	this->casings[index].thickness = t;
	updateSubs();
}

void PullPlan :: setCasingShape(int s, unsigned int index) {

	if (index >= this->casings.size())
		return;
	this->casings[index].shape = s;
	updateSubs();
}

void PullPlan :: setOutermostCasingShape(int s) {

	this->casings[casings.size()-1].shape = s;
	updateSubs();
}

float PullPlan :: getCasingThickness(unsigned int index) {

	if (index >= this->casings.size())
		return -1.0;
	return this->casings[index].thickness;
}

int PullPlan :: getOutermostCasingShape() {
	
	return this->casings[casings.size()-1].shape;
}

int PullPlan :: getCasingShape(unsigned int index) {
	
	if (index >= this->casings.size())
		return -1.0;
	return this->casings[index].shape;
}

void PullPlan :: pushNewSubpull(vector<SubpullTemplate>* newSubs,
	int shape, Point p, float diameter, int group) {

	if (newSubs->size() < subs.size()) {
		newSubs->push_back(SubpullTemplate(subs[newSubs->size()].plan, shape, p, diameter, group));
	}
	else { // you've run out of existing subplans copy from
		switch (shape) {
			case CIRCLE_SHAPE:
				newSubs->push_back(SubpullTemplate(defaultCircleSubplan, 
					CIRCLE_SHAPE, p, diameter, group));
				break;
			case SQUARE_SHAPE:
				newSubs->push_back(SubpullTemplate(defaultSquareSubplan, 
					SQUARE_SHAPE, p, diameter, group));
				break;
		}
	}
}

/*
updateSubs()

Description:
This function is invoked after the template, a template parameter,
or casing data is changed in the pull plan. The purpose is to
recompute the locations and sizes of subcanes, as well as add or remove
subplans if the number of subplans changed. For instance, changing
a template parameter specifying the number of subcanes in a row changes
the size and location of subplans, as well as increasing or decreasing
the number of subplans.
*/
void PullPlan :: updateSubs()
{
        Point p;
        float radius = 1.0;

	vector<SubpullTemplate> newSubs;

        p.x = p.y = p.z = 0.0;
        switch (this->templateType) {
                case CASED_CIRCLE_PULL_TEMPLATE:
                        pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, radius * 1.9, 0);
                        break;
                case CASED_SQUARE_PULL_TEMPLATE:
                        if (this->casings[0].shape == CIRCLE_SHAPE) {
                                radius *= 1.0 / pow(2, 0.5);
                        }
                        pushNewSubpull(&newSubs, SQUARE_SHAPE, p, radius * 2.0, 0);
                        break;
                case HORIZONTAL_LINE_CIRCLE_PULL_TEMPLATE: 
		{
                        int count = parameterValues[0];
                        for (int i = 0; i < count; ++i) {
                                float littleRadius = (2 * radius / count) / 2;
                                p.x = -radius + littleRadius + i * 2 * littleRadius;
                                pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, littleRadius * 2.0, 0);
                        }
                        break;
                }
                case HORIZONTAL_LINE_SQUARE_PULL_TEMPLATE:
                {
                        if (this->casings[0].shape == CIRCLE_SHAPE)
                                radius *= 0.9;

                        int count = parameterValues[0];
                        for (int i = 0; i < count; ++i) {
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
                        for (int i = 0; i < count; ++i) {
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
                        for (int i = 0; i < count; ++i) {
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
                        if (this->casings[0].shape == CIRCLE_SHAPE)
                                radius *= 1 / SQRT_TWO;

                        int count = parameterValues[0];
                        float littleRadius = radius / count;

			// We add the subtemplates in this funny way so that the
			// ith subcane is always at the same location regardless of
			// parameters. This is needed for delete to work correctly.
			for (int s = 0; s < count; ++s) {
				for (int i = 0; i < count; ++i) {
					for (int j = 0; j < count; ++j) {
						if ((i != s && j != s))
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
			for (int i = 1; i < count; ++i) {
				for (int theta = 0; theta < 3; ++theta) {
                                        p.x = (littleRadius * 2 * i) * cos(TWO_PI / 3 * theta);
                                        p.y = (littleRadius * 2 * i) * sin(TWO_PI / 3 * theta);
                                        pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, littleRadius * 2, 0);
                                }
                        }
                        break;
                }
                case SURROUNDING_SQUARE_PULL_TEMPLATE:
                {
                        if (this->casings[0].shape == CIRCLE_SHAPE)
                                radius *= 1 / SQRT_TWO;

                        int count = parameterValues[0];
                        float littleRadius = radius / (count + 2);

                        p.x = p.y = 0.0;
                        pushNewSubpull(&newSubs, SQUARE_SHAPE, p, 2 * littleRadius * count, 0);
                        for (int i = 0; i < count + 1; ++i) {
				p.x = -2 * littleRadius * (count + 1) / 2.0 + 2 * littleRadius * i;
				p.y = -2 * littleRadius * (count + 1) / 2.0;
				pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, 2 * littleRadius, 1);
                        }
                        for (int j = 0; j < count + 1; ++j) {
				p.x = -2 * littleRadius * (count + 1) / 2.0 + 2 * littleRadius * (count + 1);
				p.y = -2 * littleRadius * (count + 1) / 2.0 + 2 * littleRadius * j;
				pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, 2 * littleRadius, 1);
                        }
                        for (int i = count + 1; i >= 1; --i) {
				p.x = -2 * littleRadius * (count + 1) / 2.0 + 2 * littleRadius * i;
				p.y = -2 * littleRadius * (count + 1) / 2.0 + 2 * littleRadius * (count + 1);
				pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, 2 * littleRadius, 1);
                        }
                        for (int j = count + 1; j >= 1; --j) {
				p.x = -2 * littleRadius * (count + 1) / 2.0;
				p.y = -2 * littleRadius * (count + 1) / 2.0 + 2 * littleRadius * j;
				pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, 2 * littleRadius, 1);
                        }
                        break;
                }
        }

	subs = newSubs;
}






