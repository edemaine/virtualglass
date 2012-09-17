
#include "pullplan.h"
#include <cstdio>

#ifdef UNORDERED_MAP_WORKAROUND
#include <tr1/unordered_map>
#include <tr1/unordered_set>
using std::tr1::unordered_map;
using std::tr1::unordered_set;
#else
#include <unordered_map>
#include <unordered_set>
using std::unordered_map;
using std::unordered_set;
#endif

using std::pair;
using std::make_pair;
using std::vector;

PullPlan :: PullPlan(PullTemplate::Type templateType) {

	defaultGlassColor = new GlassColor();
	defaultCircleSubplan = defaultSquareSubplan = NULL;

	this->twist = 0.0;
	setTemplateType(templateType, true);
}


bool PullPlan :: hasDependencyOn(PullPlan* plan) {

	if (this == plan)
		return true;

	bool childrenAreDependent = false;
	for (unsigned int i = 0; i < subs.size(); ++i) {
		if (subs[i].plan->hasDependencyOn(plan)) {
			childrenAreDependent = true;
			break;
		}
	}

	return childrenAreDependent;
}

bool PullPlan :: hasDependencyOn(GlassColor* glassColor) {

	for (unsigned int i = 0; i < casings.size(); ++i) {
		if (this->casings[i].glassColor == glassColor)
			return true;
	} 

	bool childrenAreDependent = false;
	for (unsigned int i = 0; i < subs.size(); ++i) {
		if (subs[i].plan->hasDependencyOn(glassColor)) {
			childrenAreDependent = true;
			break;
		}
	}

	return childrenAreDependent;
}

void PullPlan :: setTemplateType(PullTemplate::Type templateType, bool force) {

	if (!force && templateType == this->templateType)
		return;

	this->templateType = templateType;

	// If the pull template has subplans and you
	// haven't initialized your default subplans yet, do it
	if (defaultCircleSubplan == NULL && templateType != PullTemplate::baseCircle && templateType != PullTemplate::baseSquare) {
		// initialize default subplans
		defaultCircleSubplan = new PullPlan(PullTemplate::baseCircle);
		defaultSquareSubplan = new PullPlan(PullTemplate::baseSquare);
	}

	parameterNames.clear();
	parameterValues.clear();
	char* tmp;
	casings.clear();
	casings.push_back(Casing(1.0, CIRCLE_SHAPE, defaultGlassColor));
	casings.push_back(Casing(1.0, CIRCLE_SHAPE, defaultGlassColor));
	casings[0].thickness = 0.9;
	switch (templateType) {
		case PullTemplate::baseCircle:
			break;
		case PullTemplate::baseSquare:
			casings[0].shape  = SQUARE_SHAPE;
			casings[0].thickness = 1 / SQRT_TWO;
			break;
		case PullTemplate::horizontalLineCircle:
			tmp = new char[100];
			sprintf(tmp, "Row count:");
			parameterNames.push_back(tmp);
			parameterValues.push_back(3);
			break;
		case PullTemplate::horizontalLineSquare:
			tmp = new char[100];
			sprintf(tmp, "Row count:");
			parameterNames.push_back(tmp);
			parameterValues.push_back(3);
			break;
		case PullTemplate::surroundingCircle:
			tmp = new char[100];
			sprintf(tmp, "Count:");
			parameterNames.push_back(tmp);
			parameterValues.push_back(8);
			break;
		case PullTemplate::cross:
			tmp = new char[100];
			sprintf(tmp, "Radial count:");
			parameterNames.push_back(tmp);
			parameterValues.push_back(3);
			break;
		case PullTemplate::squareOfSquares:
		case PullTemplate::squareOfCircles:
			casings[0].shape = SQUARE_SHAPE;
			casings[0].thickness = 1 / SQRT_TWO;
			tmp = new char[100];
			sprintf(tmp, "Row count:");
			parameterNames.push_back(tmp);
			parameterValues.push_back(4);
			break;
		case PullTemplate::tripod:
			tmp = new char[100];
			sprintf(tmp, "Radial count:");
			parameterNames.push_back(tmp);
			parameterValues.push_back(3);
			break;
		case PullTemplate::surroundingSquare:
			casings[0].shape = SQUARE_SHAPE;
			casings[0].thickness = 1 / SQRT_TWO;
			tmp = new char[100];
			sprintf(tmp, "Column count:");
			parameterNames.push_back(tmp);
			parameterValues.push_back(2);
			break;
		case PullTemplate::customCircle:
			break;
		case PullTemplate::customSquare:
			this->casings[0].shape = SQUARE_SHAPE;
			casings[0].thickness = 1 / SQRT_TWO;
			break;
	}

	vector<SubpullTemplate> oldSubs = subs;
	subs.clear(); // don't carry over any of the current stuff
	resetSubs(oldSubs);
}

void PullPlan :: setTemplateTypeToCustom()
{
	if (this->getCasingShape(0) == CIRCLE_SHAPE)
	{
		this->templateType = PullTemplate::customCircle;
	}
	else
	{
		this->templateType = PullTemplate::customSquare;
	}
	this->parameterNames.clear();
	this->parameterValues.clear();
}

void PullPlan :: setCasingColor(GlassColor* gc, unsigned int index) {

	if (index >= this->casings.size())
		return;
	this->casings[index].glassColor = gc;
}

void PullPlan :: setOutermostCasingColor(GlassColor* gc) {

	this->casings[casings.size()-1].glassColor = gc;
}

GlassColor* PullPlan :: getOutermostCasingColor() {

	return this->casings[casings.size()-1].glassColor;
}

GlassColor* PullPlan :: getCasingColor(unsigned int index) {

	if (index >= this->casings.size())
		return NULL;
	return this->casings[index].glassColor;
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
	vector<SubpullTemplate> oldSubs = subs;
	resetSubs(oldSubs);
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
	if (count < 3)
		return;

	float oldInnermostCasingThickness = casings[0].thickness;

	// `puff out' the casing thicknesses so second outermost now has radius of 
	// previous outermost one
	float diff = casings[count-1].thickness - casings[count-2].thickness;
	casings.pop_back();
	for (unsigned int i = 0; i < casings.size(); ++i) 
		casings[i].thickness += diff;

	// rescale subcanes
	for (unsigned int i = 0; i < subs.size(); ++i)
		subs[i].rescale(casings[0].thickness / oldInnermostCasingThickness);
}



bool PullPlan :: hasSquareCasing() {
	
	for (unsigned int i = 1; i < casings.size(); ++i) {
		if (casings[i].shape == SQUARE_SHAPE) {
			return true;
		}
	}
	return false;	
}

void PullPlan :: addCasing(int shape) {

	// rescale all but innermost casing
	float oldInnermostCasingThickness = casings[0].thickness;
	for (unsigned int i = 0; i < casings.size(); ++i) 
		casings[i].thickness -= MIN(0.1, casings[i].thickness/2);
	
	// if casing addition is circle around a square, rescale everything down a bit more
	if (shape == CIRCLE_SHAPE && getOutermostCasingShape() == SQUARE_SHAPE) {		
		for (unsigned int i = 0; i < casings.size(); ++i) {
			casings[i].thickness *= 1 / SQRT_TWO;
		}
	}

	// add the new casing
	casings.push_back(Casing(1.0, shape, defaultGlassColor));
	if (hasSquareCasing())
		this->twist = 0.0;

	// update subpulls by rescaling them according to innermost casing rescaling
	for (unsigned int i = 0; i < subs.size(); ++i) 
		subs[i].rescale(casings[0].thickness / oldInnermostCasingThickness);
}

void PullPlan :: setCasingThickness(float t, unsigned int index) {
	// this currently doesn't enforce any overlapping issues with
	// differently-shaped casings. It assumes they are being set 
	// to valid relative sizes.
	if (index >= casings.size())
		return;
	// if innermost casing, scale subcanes with changing casing thickness
	if (index == 0) {
		float scaleRatio = t / casings[0].thickness;
		casings[0].thickness = t;
		for (unsigned int i = 0; i < subs.size(); ++i) 
			subs[i].rescale(scaleRatio);
	}
	// otherwise just change the casing
	else
		this->casings[index].thickness = t;
}

void PullPlan :: setOutermostCasingShape(int newShape) {

	if (newShape == getOutermostCasingShape()) 
		return; 

	float oldInnermostCasingThickness = casings[0].thickness;

	// simple case of only 1 casing
	if (casings.size() == 1) {
		casings[0].shape = newShape;
		return;
	}

	// if we're moving from square to circle and the interior casing is square and
	// would collide with the new casing shape, scale everything down to make room
	if (newShape == CIRCLE_SHAPE && casings[casings.size()-2].shape == SQUARE_SHAPE
		&& casings[casings.size()-2].thickness > 1 / SQRT_TWO) { 
		for (unsigned int i = 0; i < casings.size() - 1; ++i) 
			casings[i].thickness *= 1 / SQRT_TWO;
	}

	// DO THE CHANGE
	casings[casings.size()-1].shape = newShape;
	if (hasSquareCasing())
		this->twist = 0.0;

	// because innermost casing thickness might have changed, update subcane scales
	for (unsigned int i = 0; i < subs.size(); ++i) 
		subs[i].rescale(casings[0].thickness / oldInnermostCasingThickness);
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

	assert(shape == CIRCLE_SHAPE || shape == SQUARE_SHAPE);

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
			default:
				//this should always push something, right?
				assert(shape == CIRCLE_SHAPE || shape == SQUARE_SHAPE);
				break;
		}
	}
}

/*
resetSubs()

Description:
This function is invoked after the template or a template parameter,
has changed in the pull plan. The purpose is to recompute the locations 
and sizes of subcanes, as well as add or remove subplans if the number 
of subplans changed. For instance, changing a template parameter 
specifying the number of subcanes in a row changes the size and location 
of subplans, as well as increasing or decreasing the number of subplans.
*/
void PullPlan :: resetSubs(vector<SubpullTemplate> oldSubs)
{
	Point p = make_vector(0.0f, 0.0f, 0.0f);
	assert(!casings.empty());
	float radius = casings[0].thickness;

	vector<SubpullTemplate> newSubs;

	switch (this->templateType) {
		case PullTemplate::baseCircle:
		case PullTemplate::baseSquare:
			break;
		case PullTemplate::horizontalLineCircle:
		{
			assert(parameterValues.size() == 1);
			int count = parameterValues[0];
			for (int i = 0; i < count; ++i) {
				float littleRadius = (2 * radius / count) / 2;
				p.x = -radius + littleRadius + i * 2 * littleRadius;
				pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, littleRadius * 2.0, 0);
			}
			break;
		}
		case PullTemplate::horizontalLineSquare:
		{
			assert(parameterValues.size() == 1);
			radius *= 0.9;
			int count = parameterValues[0];
			for (int i = 0; i < count; ++i) {
				float littleRadius = (2 * radius / count) / 2;
				p.x = -radius + littleRadius + i * 2 * littleRadius;
				pushNewSubpull(&newSubs, SQUARE_SHAPE, p, littleRadius * 2.0, 0);
			}
			break;
		}
		case PullTemplate::surroundingCircle:
		{
			assert(parameterValues.size() == 1);
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
		case PullTemplate::cross:
		{
			assert(parameterValues.size() == 1);
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
		case PullTemplate::squareOfCircles:
		case PullTemplate::squareOfSquares:
		{
			assert(parameterValues.size() == 1);
			if (this->casings[0].shape == CIRCLE_SHAPE)
				radius *= 1 / SQRT_TWO;

			int count = parameterValues[0];
			float littleRadius = radius / count;

			// We add the subtemplates in this funny way so that the
			// ith subcane is always at the same location regardless of
			// parameters. This is needed for delete to work correctly.
			for (int s = 0; s < count; ++s) {
				int i, j;
				for (i = 0; i < s; ++i) {
					j = s;
					p.x = -radius + littleRadius + 2 * littleRadius * i;
					p.y = -radius + littleRadius + 2 * littleRadius * j;
					if (this->templateType == PullTemplate::squareOfCircles)
						pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, 2 * littleRadius, 0);
					else
						pushNewSubpull(&newSubs, SQUARE_SHAPE, p, 2 * littleRadius, 0);
				}
				for (j = s; j >= 0; --j) {
					i = s;
					p.x = -radius + littleRadius + 2 * littleRadius * i;
					p.y = -radius + littleRadius + 2 * littleRadius * j;
					if (this->templateType == PullTemplate::squareOfCircles)
						pushNewSubpull(&newSubs, CIRCLE_SHAPE, p, 2 * littleRadius, 0);
					else
						pushNewSubpull(&newSubs, SQUARE_SHAPE, p, 2 * littleRadius, 0);
				}
				
			}
			break;
		}
		case PullTemplate::tripod:
		{
			assert(parameterValues.size() == 1);
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
		case PullTemplate::surroundingSquare:
		{
			assert(parameterValues.size() == 1);
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
		case PullTemplate::customCircle:
		case PullTemplate::customSquare:
		{
			for (unsigned int i = 0; i < oldSubs.size(); i++)
			{
				p.x = oldSubs[i].location.x * radius;
				p.y = oldSubs[i].location.y * radius;
				pushNewSubpull(&newSubs, oldSubs[i].shape, oldSubs[i].location, 
					oldSubs[i].diameter, oldSubs[i].group);
			}
			break;
		}
	}

	subs = newSubs;
}

PullPlan* PullPlan :: copy() const {

	PullPlan* c = new PullPlan(templateType);

	c->casings = casings;
	c->twist = twist;

	assert(c->parameterValues.size() == parameterValues.size());
	c->parameterValues = parameterValues;
	vector<SubpullTemplate> oldSubs = subs;
	c->resetSubs(oldSubs);

	assert(c->subs.size() == subs.size());
	for (unsigned int i = 0; i < subs.size(); ++i) {
		c->subs[i].plan = subs[i].plan;
	}

	return c;
}

PullPlan *deep_copy(const PullPlan *_plan) {
	unordered_map< const PullPlan *, PullPlan * > copies;
	PullPlan *plan = _plan->copy();
	copies.insert(make_pair(_plan, plan));

	vector< PullPlan * > to_update;
	to_update.push_back(plan);
	//update sub-templates to point to copies as well:
	while (!to_update.empty()) {
		PullPlan *t = to_update.back();
		to_update.pop_back();
		for (vector< SubpullTemplate >::iterator s = t->subs.begin(); s != t->subs.end(); ++s) {
			unordered_map< const PullPlan *, PullPlan * >::iterator f = copies.find(s->plan);
			if (f == copies.end()) {
				f = copies.insert(make_pair(s->plan, s->plan->copy())).first;
				to_update.push_back(f->second);
			}
			s->plan = f->second;
		}
	}
	return plan;
}

void deep_delete(PullPlan *plan) {
	//Because pull plans don't delete their children (which is right):
	unordered_set< PullPlan * > marked;
	vector< PullPlan * > to_delete;
	to_delete.push_back(plan);
	while (!to_delete.empty()) {
		PullPlan *t = to_delete.back();
		to_delete.pop_back();
		for (vector< SubpullTemplate >::iterator s = t->subs.begin(); s != t->subs.end(); ++s) {
			if (marked.insert(s->plan).second) {
				to_delete.push_back(s->plan);
			}
			s->plan = NULL;
		}
		delete t;
	}
}
