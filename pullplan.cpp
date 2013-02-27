
#include "casing.h"
#include "glasscolor.h"
#include "constants.h"
#include "templateparameter.h"
#include "globalglass.h"
#include "pullplan.h"

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

PullPlan :: PullPlan(PullTemplate::Type _templateType)
{
	// setup default twist
	twist = 0;

	// initialize casings and subplans to something simple
	casings.push_back(Casing(1.0, CIRCLE_SHAPE, GlobalGlass::color()));
	casings[0].shape = CIRCLE_SHAPE;
	count = 0;
	this->templateType = PullTemplate::BASE_CIRCLE;

	// now initialize for real
	setTemplateType(_templateType);
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

void PullPlan :: setTemplateType(PullTemplate::Type templateType) 
{
	if (templateType == this->templateType)
		return;

	// if you're switching to a template where count matters, and it's a funky value,
	// set it to something more reasonable
	if (this->count < 2 && templateHasSubplans(templateType))
		this->count = 3;

	this->templateType = templateType;

	switch (templateType) 
	{
		case PullTemplate::BASE_CIRCLE:
			casings[0].shape = CIRCLE_SHAPE;
			break;
		case PullTemplate::BASE_SQUARE:
			casings[0].shape = SQUARE_SHAPE;
			if (casings.size() > 1) 
				casings[0].thickness = MIN(casings[0].thickness, 0.9 * 1 / SQRT_TWO * casings[1].thickness);
			break;
		case PullTemplate::HORIZONTAL_LINE_CIRCLE:
			casings[0].shape = CIRCLE_SHAPE;
			break;
		case PullTemplate::HORIZONTAL_LINE_SQUARE:
			casings[0].shape  = CIRCLE_SHAPE;
			break;
		case PullTemplate::SURROUNDING_CIRCLE:
			casings[0].shape  = CIRCLE_SHAPE;
			break;
		case PullTemplate::CROSS:
			casings[0].shape  = CIRCLE_SHAPE;
			break;
		case PullTemplate::SQUARE_OF_SQUARES:
		case PullTemplate::SQUARE_OF_CIRCLES:
			casings[0].shape = SQUARE_SHAPE;
			if (casings.size() > 1)
				casings[0].thickness = MIN(casings[0].thickness, 0.9 * 1 / SQRT_TWO * casings[1].thickness);
			break;
		case PullTemplate::TRIPOD:
			casings[0].shape  = CIRCLE_SHAPE;
			break;
		case PullTemplate::SURROUNDING_SQUARE:
			casings[0].shape = SQUARE_SHAPE;
			if (casings.size() > 1)
				casings[0].thickness = MIN(casings[0].thickness, 0.9 * 1 / SQRT_TWO * casings[1].thickness);
			break;
		case PullTemplate::CUSTOM:
			// we don't touch anything, who knows what's going on in there
			break;
	}

	resetSubs(true);
}

void PullPlan :: setCasingColor(GlassColor* gc, unsigned int index) {
	if (index >= this->casings.size())
		return;
	this->casings[index].glassColor = gc;
}

void PullPlan :: setOutermostCasingColor(GlassColor* gc) 
{
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

bool PullPlan :: hasMinimumCasingCount() {
	return (this->casings.size() < 2);
}	
		

unsigned int PullPlan :: getCasingCount() {

	return this->casings.size();
}

enum PullTemplate::Type PullPlan :: getTemplateType() {

	return this->templateType;
}

unsigned int PullPlan :: getCount()
{
	return this->count;
}

void PullPlan :: setCount(unsigned int _count)
{
	this->count = _count;
	resetSubs(false);
}

void PullPlan :: removeCasing() 
{
	int count = casings.size();
	if (count < 2) 
		return;

	float oldInnermostCasingThickness = casings[0].thickness;

	// "puff out" the casing thicknesses so second outermost now has radius of 
	// previous outermost one
	float diff = 1.0 - casings[count-2].thickness;
	casings.pop_back();
	casings[count-2].thickness = 1.0;
	for (unsigned int i = 0; i < casings.size()-1; ++i) 
	{
		if (casings[i].shape == casings[i+1].shape || casings[i].shape == CIRCLE_SHAPE)
			casings[i].thickness += diff;
		else
			casings[i].thickness += diff/SQRT_TWO;
	}

	// rescale subcanes
	for (unsigned int i = 0; i < subs.size(); ++i)
		subs[i].rescale(casings[0].thickness / oldInnermostCasingThickness);
}


void PullPlan :: addCasing(enum GeometricShape _shape) {

	// rescale casings
	float oldInnermostCasingThickness = casings[0].thickness;
	for (unsigned int i = 0; i < casings.size(); ++i) 
	{
		switch (casings[i].shape)
		{
			case CIRCLE_SHAPE:
				casings[i].thickness -= MIN(0.1, casings[i].thickness/2);
				break;
			case SQUARE_SHAPE:
				casings[i].thickness -= MIN(0.1 / SQRT_TWO, casings[i].thickness/2);
				break;
		}
	}
	
	// if casing addition is circle around a square, rescale everything down a bit more
	if (_shape == CIRCLE_SHAPE && getOutermostCasingShape() == SQUARE_SHAPE) {		
		for (unsigned int i = 0; i < casings.size(); ++i) {
			casings[i].thickness *= 1 / SQRT_TWO;
		}
	}

	// add the new casing
	casings.push_back(Casing(1.0, _shape, GlobalGlass::color()));
	if (this->getOutermostCasingShape() != CIRCLE_SHAPE)
		this->twist = 0.0;

	// update subpulls by rescaling them according to innermost casing rescaling
	for (unsigned int i = 0; i < subs.size(); ++i) 
		subs[i].rescale(casings[0].thickness / oldInnermostCasingThickness);
}

void PullPlan :: setCasingThickness(float t, unsigned int index) {
	// this currently doesn't enforce any overlapping issues with
	// differently-shaped casings. It assumes they are being set 
	// to valid relative sizes.
	if (index >= casings.size()-1)
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

void PullPlan :: setOutermostCasingShape(enum GeometricShape _shape) 
{
	if (_shape == getOutermostCasingShape()) 
		return;

	if (this->casings.size() > 1) 
	{
		// if we're moving from square to circle and the interior casing is square and
		// would collide with the new casing shape, scale everything down to make room
		if (_shape == CIRCLE_SHAPE && this->casings[this->casings.size()-2].shape == SQUARE_SHAPE
			&& this->casings[this->casings.size()-2].thickness > 1 / SQRT_TWO) 
		{ 
			for (unsigned int i = 0; i < casings.size() - 1; ++i) 
				this->casings[i].thickness *= 1 / SQRT_TWO;
		}
	}
	else 
	{
		// special case: if you only have one casing and your template is 
		// "I am this shape" (e.g. BASE_*), then changing casing shape should
		// change template type implicitly, since a a BASE_CIRCLE consisting of 
		// a single SQUARE_SHAPE casing makes no sense
		if (this->templateType == PullTemplate::BASE_CIRCLE && _shape == SQUARE_SHAPE)
			this->setTemplateType(PullTemplate::BASE_SQUARE);
		else if (this->templateType == PullTemplate::BASE_SQUARE && _shape == CIRCLE_SHAPE)
			this->setTemplateType(PullTemplate::BASE_CIRCLE);
	}

	// DO THE CHANGE
	casings[casings.size()-1].shape = _shape;
	if (this->getOutermostCasingShape() != CIRCLE_SHAPE)
		this->twist = 0.0;

	resetSubs(false);
}

float PullPlan :: getCasingThickness(unsigned int index) {

	return this->casings[index].thickness;
}

enum GeometricShape PullPlan :: getOutermostCasingShape() {
	
	return this->casings[casings.size()-1].shape;
}

enum GeometricShape PullPlan :: getCasingShape(unsigned int index) {
	
	return this->casings[index].shape;
}

void PullPlan :: pushNewSubpull(bool hardReset, vector<SubpullTemplate>* newSubs,
	enum GeometricShape _shape, Point2D p, float diameter) 
{
	PullPlan* plan = 0;

	// if it's not a hard reset and there are still old subplans to use and the next one matches shape
	// with the shape we want to have, then use it
	if (!hardReset && newSubs->size() < subs.size() && _shape == subs[newSubs->size()].shape) 
	{
		plan = subs[newSubs->size()].plan;
	}
	else // otherwise just use whichever filler subplan matches the shape
	{
		switch (_shape) 
		{
			case CIRCLE_SHAPE:
				plan = GlobalGlass::circlePlan();
				break;
			case SQUARE_SHAPE:
				plan = GlobalGlass::squarePlan();
				break;
		}
	}

	newSubs->push_back(SubpullTemplate(plan, _shape, p, diameter));
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
void PullPlan :: resetSubs(bool hardReset)
{
	Point2D p = make_vector(0.0f, 0.0f);
	float radius = casings[0].thickness;

	vector<SubpullTemplate> newSubs;
	
	switch (templateType) 
	{
		case PullTemplate::BASE_CIRCLE:
		case PullTemplate::BASE_SQUARE:
			break;
		case PullTemplate::HORIZONTAL_LINE_CIRCLE:
		{
			for (unsigned int i = 0; i < this->count; ++i) 
			{
				float littleRadius = (2 * radius / this->count) / 2;
				p.x = -radius + littleRadius + i * 2 * littleRadius;
				pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, littleRadius * 2.0);
			}
			break;
		}
		case PullTemplate::HORIZONTAL_LINE_SQUARE:
		{
			radius *= 0.9;
			for (unsigned int i = 0; i < this->count; ++i) 
			{
				float littleRadius = (2 * radius / this->count) / 2;
				p.x = -radius + littleRadius + i * 2 * littleRadius;
				pushNewSubpull(hardReset, &newSubs, SQUARE_SHAPE, p, littleRadius * 2.0);
			}
			break;
		}
		case PullTemplate::SURROUNDING_CIRCLE:
		{
			float theta = TWO_PI / this->count;
			float k = sin(theta/2) / (1 + sin(theta/2));

			p.x = p.y = 0.0;
			pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, (1 - 2 * k) * 2 * radius);
			for (unsigned int i = 0; i < this->count; ++i) 
			{
				p.x = (1.0 - k) * radius * cos(TWO_PI / this->count * i);
				p.y = (1.0 - k) * radius * sin(TWO_PI / this->count * i);
				pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, 2 * k * radius);
			}
			break;
		}
		case PullTemplate::CROSS:
		{
			float littleRadius = (radius / (this->count + 0.5)) / 2.0;

			p.x = p.y = 0.0;
			pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, littleRadius * 2.0);
			for (unsigned int i = 0; i < this->count; ++i) 
			{
				p.x = (i+1) * 2 * littleRadius;
				p.y = 0.0;
				pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, littleRadius * 2.0);
				p.x = 0.0;
				p.y = (i+1) * 2 * littleRadius;
				pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, littleRadius * 2.0);
				p.x = -((i+1) * 2 * littleRadius);
				p.y = 0.0;
				pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, littleRadius * 2.0);
				p.x = 0.0;
				p.y = -((i+1) * 2 * littleRadius);
				pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, littleRadius * 2.0);
			}
			break;
		}
		case PullTemplate::SQUARE_OF_CIRCLES:
		case PullTemplate::SQUARE_OF_SQUARES:
		{
			if (this->casings[0].shape == CIRCLE_SHAPE)
				radius *= 1 / SQRT_TWO;

			float littleRadius = radius / this->count;

			// We add the subtemplates in this funny way so that the
			// ith subcane is always at the same location regardless of
			// parameters. This is needed for delete to work correctly.
			for (unsigned int s = 0; s < this->count; ++s) 
			{
				unsigned int i, j;
				for (unsigned int i = 0; i < s; ++i) 
				{
					j = s;
					p.x = -radius + littleRadius + 2 * littleRadius * i;
					p.y = -radius + littleRadius + 2 * littleRadius * j;
					if (this->templateType == PullTemplate::SQUARE_OF_CIRCLES)
						pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, 2 * littleRadius);
					else
						pushNewSubpull(hardReset, &newSubs, SQUARE_SHAPE, p, 2 * littleRadius);
				}
				for (j = s; j <= s; --j) 
				{
					i = s;
					p.x = -radius + littleRadius + 2 * littleRadius * i;
					p.y = -radius + littleRadius + 2 * littleRadius * j;
					if (this->templateType == PullTemplate::SQUARE_OF_CIRCLES)
						pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, 2 * littleRadius);
					else
						pushNewSubpull(hardReset, &newSubs, SQUARE_SHAPE, p, 2 * littleRadius);
				}
				
			}
			break;
		}
		case PullTemplate::TRIPOD:
		{
			float littleRadius = radius / (2 * this->count - 1);

			p.x = p.y = 0.0;
			pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, 2 * littleRadius);
			for (unsigned int i = 1; i < this->count; ++i) 
			{
				for (unsigned int theta = 0; theta < 3; ++theta) 
				{
					p.x = (littleRadius * 2 * i) * cos(TWO_PI / 3 * theta);
					p.y = (littleRadius * 2 * i) * sin(TWO_PI / 3 * theta);
					pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, littleRadius * 2);
				}
			}
			break;
		}
		case PullTemplate::SURROUNDING_SQUARE:
		{
			if (this->casings[0].shape == CIRCLE_SHAPE)
				radius *= 1 / SQRT_TWO;

			float littleRadius = radius / (this->count + 2);

			p.x = p.y = 0.0;
			pushNewSubpull(hardReset, &newSubs, SQUARE_SHAPE, p, 2 * littleRadius * this->count);
			for (unsigned int i = 0; i < this->count + 1; ++i) 
			{
				p.x = -2 * littleRadius * (this->count + 1) / 2.0 + 2 * littleRadius * i;
				p.y = -2 * littleRadius * (this->count + 1) / 2.0;
				pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, 2 * littleRadius);
			}
			for (unsigned int j = 0; j < this->count + 1; ++j) 
			{
				p.x = -2 * littleRadius * (this->count + 1) / 2.0 + 2 * littleRadius * (this->count + 1);
				p.y = -2 * littleRadius * (this->count + 1) / 2.0 + 2 * littleRadius * j;
				pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, 2 * littleRadius);
			}
			for (unsigned int i = this->count + 1; i >= 1; --i) 
			{
				p.x = -2 * littleRadius * (this->count + 1) / 2.0 + 2 * littleRadius * i;
				p.y = -2 * littleRadius * (this->count + 1) / 2.0 + 2 * littleRadius * (this->count + 1);
				pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, 2 * littleRadius);
			}
			for (unsigned int j = this->count + 1; j >= 1; --j) 
			{
				p.x = -2 * littleRadius * (this->count + 1) / 2.0;
				p.y = -2 * littleRadius * (this->count + 1) / 2.0 + 2 * littleRadius * j;
				pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, 2 * littleRadius);
			}
			break;
		}
		case PullTemplate::CUSTOM:
		{
			for (unsigned int i = 0; i < subs.size(); i++)
			{
				p.x = subs[i].location.x * radius;
				p.y = subs[i].location.y * radius;
				// never do a hard reset of custom, because 
				// it's a `soft' template change from a rigid one 
				// to a custom one, so mapping from old cane locations/subcanes
				// to new ones is direct and very natural
				pushNewSubpull(false, &newSubs, subs[i].shape, subs[i].location, subs[i].diameter);
			}
			break;
		}
	}

	subs = newSubs;
}

PullPlan* PullPlan :: copy() const 
{

	PullPlan* c = new PullPlan(templateType);

	c->casings = casings;
	c->twist = twist;
	c->count = count;
	// need to copy SubpullTemplate list in the case that it's a custom
	// template, in which case the only record of the subpull
	// location, shape, size, etc. is the SubpullTemplate list
	c->subs.clear();
	for (unsigned int i = 0; i < subs.size(); ++i)
	{
		c->subs.push_back(subs[i]);
	}
		
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
