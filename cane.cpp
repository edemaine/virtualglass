
#include "casing.h"
#include "glasscolor.h"
#include "constants.h"
#include "templateparameter.h"
#include "globalglass.h"
#include "cane.h"

#include <tr1/unordered_map>
#include <tr1/unordered_set>
using std::tr1::unordered_map;
using std::tr1::unordered_set;

using std::pair;
using std::make_pair;

Cane :: Cane(PullTemplate::Type _templateType)
{
	// setup default twist
	this->state.twist = 0;

	// initialize casings and subplans to something simple
	this->state.casings.push_back(Casing(1.0, CIRCLE_SHAPE, GlobalGlass::color()));
	this->state.casings[0].shape = CIRCLE_SHAPE;
	this->state.count = 0;
	this->state.type = PullTemplate::BASE_CIRCLE;

	// now initialize for real
	setTemplateType(_templateType);

	undoStack.push(this->state);
}

void Cane :: undo()
{
	if (!canUndo())
		return;
	redoStack.push(undoStack.top());
	undoStack.pop();
        this->state = undoStack.top();
}

void Cane :: redo()
{
	if (!canRedo())
		return;
	undoStack.push(redoStack.top());
	redoStack.pop();
	this->state = undoStack.top();
}

bool Cane :: canUndo()
{
	return (undoStack.size() >= 2);
}

bool Cane :: canRedo()
{
	return (redoStack.size() > 0);
}

void Cane :: saveState()
{
	undoStack.push(this->state);
	while (redoStack.size() > 0)
		redoStack.pop();
}

bool Cane :: hasDependencyOn(Cane* plan) 
{
	if (this == plan)
		return true;

	bool childrenAreDependent = false;
	for (unsigned int i = 0; i < this->state.subs.size(); ++i) 
		childrenAreDependent = (childrenAreDependent || this->state.subs[i].plan->hasDependencyOn(plan)); 

	return childrenAreDependent;
}

bool Cane :: hasDependencyOn(GlassColor* glassColor) 
{
	for (unsigned int i = 0; i < this->state.casings.size(); ++i) 
	{
		if (this->state.casings[i].glassColor == glassColor)
			return true;
	} 

	bool childrenAreDependent = false;
	for (unsigned int i = 0; i < this->state.subs.size(); ++i) 
		childrenAreDependent = (childrenAreDependent || this->state.subs[i].plan->hasDependencyOn(glassColor)); 

	return childrenAreDependent;
}

void Cane :: setTemplateType(PullTemplate::Type _templateType) 
{
	if (_templateType == this->state.type)
		return;

	// if you're switching to a template where count matters, and it's a funky value,
	// set it to something more reasonable
	if (this->state.count < 2 && templateHasSubplans(_templateType))
		this->state.count = 7;

	this->state.type = _templateType;

	vector<Casing> &casings = this->state.casings;
	switch (_templateType) 
	{
		case PullTemplate::BASE_CIRCLE:
			casings[0].shape = CIRCLE_SHAPE;
			break;
		case PullTemplate::BASE_SQUARE:
			casings[0].shape = SQUARE_SHAPE;
			this->state.twist = 0.0;
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
		case PullTemplate::TRIPOD:
			casings[0].shape  = CIRCLE_SHAPE;
			break;
		case PullTemplate::SQUARE_OF_SQUARES:
		case PullTemplate::SQUARE_OF_CIRCLES:
			casings[0].shape = SQUARE_SHAPE;
			this->state.twist = 0.0;
			if (casings.size() > 1)
				casings[0].thickness = MIN(casings[0].thickness, 0.9 * 1 / SQRT_TWO * casings[1].thickness);
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

void Cane :: setCasingColor(GlassColor* gc, unsigned int index) 
{
	if (index >= this->state.casings.size())
		return;
	this->state.casings[index].glassColor = gc;
}

void Cane :: setOutermostCasingColor(GlassColor* gc) 
{
	int last = this->state.casings.size()-1;
	this->state.casings[last].glassColor = gc;
}

const GlassColor* Cane :: outermostCasingColor() 
{
	int last = this->state.casings.size()-1;
	return this->state.casings[last].glassColor;
}

const GlassColor* Cane :: getCasingColor(unsigned int index) 
{
	if (index >= this->state.casings.size())
		return NULL;
	return this->state.casings[index].glassColor;
}

bool Cane :: hasMinimumCasingCount() 
{
	return (this->state.casings.size() < 2);
}	
		
unsigned int Cane :: casingCount() 
{
	return this->state.casings.size();
}

enum PullTemplate::Type Cane :: templateType() const 
{
	return this->state.type;
}

unsigned int Cane :: count()
{
	return this->state.count;
}

void Cane :: setCount(unsigned int _count)
{
	this->state.count = _count;
	resetSubs(false);
}

void Cane :: removeCasing() 
{
	vector<Casing>& casings = this->state.casings;

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
	for (unsigned int i = 0; i < this->state.subs.size(); ++i)
		this->state.subs[i].rescale(casings[0].thickness / oldInnermostCasingThickness);
}


void Cane :: addCasing(enum GeometricShape _shape) 
{
	vector<Casing>& casings = this->state.casings;

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
	if (_shape == CIRCLE_SHAPE && this->outermostCasingShape() == SQUARE_SHAPE) 
	{		
		for (unsigned int i = 0; i < casings.size(); ++i) 
			casings[i].thickness *= 1 / SQRT_TWO;
	}

	// add the new casing
	casings.push_back(Casing(1.0, _shape, GlobalGlass::color()));
	if (this->outermostCasingShape() != CIRCLE_SHAPE)
		this->state.twist = 0.0;

	// update subpulls by rescaling them according to innermost casing rescaling
	for (unsigned int i = 0; i < this->state.subs.size(); ++i) 
		this->state.subs[i].rescale(casings[0].thickness / oldInnermostCasingThickness);
}

void Cane :: setCasingThickness(float t, unsigned int index) 
{
	vector<Casing> &casings = this->state.casings;	

	// this currently doesn't enforce any overlapping issues with
	// differently-shaped casings. It assumes they are being set 
	// to valid relative sizes.
	if (index >= casings.size()-1)
		return;
	// if innermost casing, scale subcanes with changing casing thickness
	if (index == 0) 
	{
		float scaleRatio = t / casings[0].thickness;
		casings[0].thickness = t;
		for (unsigned int i = 0; i < this->state.subs.size(); ++i) 
			this->state.subs[i].rescale(scaleRatio);
	}
	// otherwise just change the casing
	else
		this->state.casings[index].thickness = t;
}

void Cane :: setOutermostCasingShape(enum GeometricShape _shape) 
{
	vector<Casing> &casings = this->state.casings;	

	if (_shape == this->outermostCasingShape()) 
		return;

	if (casings.size() > 1) 
	{
		// if we're moving from square to circle and the interior casing is square and
		// would collide with the new casing shape, scale everything down to make room
		if (_shape == CIRCLE_SHAPE && casings[casings.size()-2].shape == SQUARE_SHAPE
			&& casings[casings.size()-2].thickness > 1 / SQRT_TWO) 
		{ 
			for (unsigned int i = 0; i < casings.size() - 1; ++i) 
				casings[i].thickness *= 1 / SQRT_TWO;
		}
	}
	else 
	{
		// special case: if you only have one casing and your template is 
		// "I am this shape" (e.g. BASE_*), then changing casing shape should
		// change template type implicitly, since a a BASE_CIRCLE consisting of 
		// a single SQUARE_SHAPE casing makes no sense
		if (this->state.type == PullTemplate::BASE_CIRCLE && _shape == SQUARE_SHAPE)
			this->setTemplateType(PullTemplate::BASE_SQUARE);
		else if (this->state.type == PullTemplate::BASE_SQUARE && _shape == CIRCLE_SHAPE)
			this->setTemplateType(PullTemplate::BASE_CIRCLE);
	}

	// DO THE CHANGE
	casings[casings.size()-1].shape = _shape;
	if (this->outermostCasingShape() != CIRCLE_SHAPE)
		this->state.twist = 0.0;

	resetSubs(false);
}

float Cane :: getCasingThickness(unsigned int index) 
{
	return this->state.casings[index].thickness;
}

enum GeometricShape Cane :: outermostCasingShape() 
{
	int last = this->state.casings.size()-1;
	return this->state.casings[last].shape;
}

enum GeometricShape Cane :: getCasingShape(unsigned int index) 
{
	return this->state.casings[index].shape;
}

void Cane :: pushNewSubpull(bool hardReset, vector<SubpullTemplate>* newSubs,
	enum GeometricShape _shape, Point2D p, float diameter) 
{
	Cane* plan = NULL;

	// if it's not a hard reset and there are still old subplans to use and the next one matches shape
	// with the shape we want to have, then use it
	if (!hardReset && newSubs->size() < this->state.subs.size() 
		&& _shape == this->state.subs[newSubs->size()].shape) 
	{
		plan = this->state.subs[newSubs->size()].plan;
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

// resetSubs()

// Description:
// This function is invoked after the template or a template parameter,
// has changed in the pull plan. The purpose is to recompute the locations 
// and sizes of subcanes, as well as add or remove subplans if the number 
// of subplans changed. For instance, changing a template parameter 
// specifying the number of subcanes in a row changes the size and location 
// of subplans, as well as increasing or decreasing the number of subplans.
void Cane :: resetSubs(bool hardReset)
{
	Point2D p = make_vector(0.0f, 0.0f);
	float radius = this->state.casings[0].thickness;

	vector<SubpullTemplate> newSubs;
	
	switch (this->state.type) 
	{
		case PullTemplate::BASE_CIRCLE:
		case PullTemplate::BASE_SQUARE:
			break;
		case PullTemplate::HORIZONTAL_LINE_CIRCLE:
		{
			for (unsigned int i = 0; i < this->state.count; ++i) 
			{
				float littleRadius = (2 * radius / this->state.count) / 2;
				p.x = -radius + littleRadius + i * 2 * littleRadius;
				pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, littleRadius * 2.0);
			}
			break;
		}
		case PullTemplate::HORIZONTAL_LINE_SQUARE:
		{
			radius *= 0.9;
			for (unsigned int i = 0; i < this->state.count; ++i) 
			{
				float littleRadius = (2 * radius / this->state.count) / 2;
				p.x = -radius + littleRadius + i * 2 * littleRadius;
				pushNewSubpull(hardReset, &newSubs, SQUARE_SHAPE, p, littleRadius * 2.0);
			}
			break;
		}
		case PullTemplate::SURROUNDING_CIRCLE:
		{
			if (this->state.count == 0)
				break;

			unsigned int littleCount = MAX(this->state.count-1, 3);
			float theta = TWO_PI / littleCount;
			float k = sin(theta/2) / (1 + sin(theta/2));
			pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, (1 - 2 * k) * 2 * radius);
			p.x = p.y = 0.0;
			for (unsigned int i = 0; i < littleCount; ++i) 
			{
				p.x = (1.0 - k) * radius * cos(TWO_PI / littleCount * i);
				p.y = (1.0 - k) * radius * sin(TWO_PI / littleCount * i);
				pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, 2 * k * radius);
			}
			break;
		}
		case PullTemplate::CROSS:
		case PullTemplate::TRIPOD:
		{
			if (this->state.count == 0)
				break;

			unsigned int wings = 3 + static_cast<int>(this->state.type == PullTemplate::CROSS);
			unsigned int sideCount = (this->state.count + 1) / wings; 
			float littleRadius = (radius / (sideCount + 0.5)) / 2.0;
	
			p.x = p.y = 0.0;
			pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, littleRadius * 2.0);
			for (unsigned int i = 0; i < sideCount; ++i) 
			{
				for (unsigned int theta = 0; theta < wings; ++theta) 
				{
					p.x = (littleRadius * 2 * (i+1)) * cos(TWO_PI / wings * theta);
					p.y = (littleRadius * 2 * (i+1)) * sin(TWO_PI / wings * theta);
					pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, littleRadius * 2);
				}
			}
			break;
		}
		case PullTemplate::SQUARE_OF_CIRCLES:
		case PullTemplate::SQUARE_OF_SQUARES:
		{
			if (this->state.count == 0)
				break;

			if (this->state.casings[0].shape == CIRCLE_SHAPE)
				radius *= 1 / SQRT_TWO;

			unsigned int sideCount = 0;
			while (sideCount * sideCount < this->state.count)
				++sideCount;
			float littleRadius = radius / sideCount;

			// We add the subtemplates in this funny way so that the
			// ith subcane is always at the same location regardless of
			// parameters. This is needed for delete to work correctly.
			for (unsigned int s = 0; s < sideCount; ++s)
			{
				unsigned int i, j;
				for (unsigned int i = 0; i < s; ++i) 
				{
					j = s;
					p.x = -radius + littleRadius + 2 * littleRadius * i;
					p.y = -radius + littleRadius + 2 * littleRadius * j;
					if (this->state.type == PullTemplate::SQUARE_OF_CIRCLES)
						pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, 2 * littleRadius);
					else
						pushNewSubpull(hardReset, &newSubs, SQUARE_SHAPE, p, 2 * littleRadius);
				}
				for (j = s; j <= s; --j) 
				{
					i = s;
					p.x = -radius + littleRadius + 2 * littleRadius * i;
					p.y = -radius + littleRadius + 2 * littleRadius * j;
					if (this->state.type == PullTemplate::SQUARE_OF_CIRCLES)
						pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, 2 * littleRadius);
					else
						pushNewSubpull(hardReset, &newSubs, SQUARE_SHAPE, p, 2 * littleRadius);
				}
				
			}
			break;
		}
		case PullTemplate::SURROUNDING_SQUARE:
		{
			if (this->state.count == 0)
				break;

			// (1-8) : 2, (9-12) : 3, (13-16) : 4
			unsigned int sideCount = (MAX(this->state.count, 5) + 3) / 4; 

			if (this->state.casings[0].shape == CIRCLE_SHAPE)
				radius *= 1 / SQRT_TWO;

			float littleRadius = radius / (sideCount + 1);

			p.x = p.y = 0.0;
			pushNewSubpull(hardReset, &newSubs, SQUARE_SHAPE, p, 
				2 * littleRadius * (sideCount-1));
			for (unsigned int i = 0; i < sideCount; ++i)
			{
				p.x = -2 * littleRadius * sideCount / 2.0 + 2 * littleRadius * i;
				p.y = -2 * littleRadius * sideCount / 2.0;
				pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, 2 * littleRadius);
			}
			for (unsigned int j = 0; j < sideCount; ++j)
			{
				p.x = -2 * littleRadius * sideCount / 2.0 + 2 * littleRadius * sideCount; 
				p.y = -2 * littleRadius * sideCount / 2.0 + 2 * littleRadius * j;
				pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, 2 * littleRadius);
			}
			for (unsigned int i = sideCount; i >= 1; --i) 
			{
				p.x = -2 * littleRadius * sideCount / 2.0 + 2 * littleRadius * i;
				p.y = -2 * littleRadius * sideCount / 2.0 + 2 * littleRadius * sideCount;
				pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, 2 * littleRadius);
			}
			for (unsigned int j = sideCount; j >= 1; --j) 
			{
				p.x = -2 * littleRadius * sideCount / 2.0;
				p.y = -2 * littleRadius * sideCount / 2.0 + 2 * littleRadius * j;
				pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, 2 * littleRadius);
			}
			break;
		}
		case PullTemplate::CUSTOM:
		{
			for (unsigned int i = 0; i < this->state.subs.size(); i++)
			{
				p.x = this->state.subs[i].location.x * radius;
				p.y = this->state.subs[i].location.y * radius;
				// never do a hard reset of custom, because 
				// it's a `soft' template change from a rigid one 
				// to a custom one, so mapping from old cane locations/subcanes
				// to new ones is direct and very natural
				pushNewSubpull(false, &newSubs, this->state.subs[i].shape, this->state.subs[i].location, 
					this->state.subs[i].diameter);
			}
			break;
		}
	}

	this->state.subs = newSubs;
}

SubpullTemplate Cane :: getSubpullTemplate(unsigned int index)
{
	return this->state.subs[index];
}

void Cane :: setSubpullTemplate(SubpullTemplate t, unsigned int index)
{
	this->state.subs[index] = t;
}

void Cane :: addSubpullTemplate(SubpullTemplate t)
{
	this->state.subs.push_back(t);
}

void Cane :: removeSubpullTemplate(unsigned int index)
{
	this->state.subs.erase(this->state.subs.begin() + index);
}

unsigned int Cane :: subpullCount()
{
	return this->state.subs.size();
}

float Cane :: twist()
{
	return this->state.twist;
}

void Cane :: setTwist(float t)
{
	this->state.twist = t;
}

float* Cane :: twistPtr()
{
	return &(this->state.twist);
}

Cane* Cane :: copy() const 
{
	Cane* c = new Cane(this->state.type);
	c->state = this->state;
	return c;
}

Cane *deep_copy(const Cane *_plan) 
{
	unordered_map<const Cane*, Cane*> copies;
	Cane *plan = _plan->copy();
	copies.insert(make_pair(_plan, plan));

	vector<Cane*> to_update;
	to_update.push_back(plan);
	//update sub-templates to point to copies as well:
	while (!to_update.empty()) 
	{
		Cane *t = to_update.back();
		to_update.pop_back();
		for (unsigned int i = 0; i < t->subpullCount(); ++i)
		{
			SubpullTemplate s = t->getSubpullTemplate(i);
			unordered_map<const Cane*, Cane*>::iterator f = copies.find(s.plan);
			if (f == copies.end()) 
			{
				f = copies.insert(make_pair(s.plan, s.plan->copy())).first;
				to_update.push_back(f->second);
			}
			s.plan = f->second;
			t->setSubpullTemplate(s, i);
		}
	}
	return plan;
}

void deep_delete(Cane *plan) 
{
	//Because pull plans don't delete their children (which is right):
	unordered_set<Cane*> marked;
	vector<Cane*> to_delete;
	to_delete.push_back(plan);
	while (!to_delete.empty()) 
	{
		Cane *t = to_delete.back();
		to_delete.pop_back();
		for (unsigned int i = 0; i < t->subpullCount(); ++i)
		{
			SubpullTemplate s = t->getSubpullTemplate(i);
			if (marked.insert(s.plan).second) 
				to_delete.push_back(s.plan);
			s.plan = NULL;
			t->setSubpullTemplate(s, i);
		}
		delete t;
	}
}
