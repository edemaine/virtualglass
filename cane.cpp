
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

Cane :: Cane(CaneTemplate::Type t)
{
	// setup default twist
	this->_twist = 0;

	// initialize casings and subcanes to something simple
	this->_casings.push_back(Casing(1.0, CIRCLE_SHAPE, GlobalGlass::color()));
	this->_casings[0].shape = CIRCLE_SHAPE;
	this->_count = 0;
	this->_type = CaneTemplate::BASE_CIRCLE;

	// now initialize for real
	setTemplateType(t);
}

bool Cane :: hasDependencyOn(Cane* cane) 
{
	if (this == cane)
		return true;

	bool childrenAreDependent = false;
	for (unsigned int i = 0; i < this->_subs.size(); ++i) 
		childrenAreDependent = (childrenAreDependent || this->_subs[i].cane->hasDependencyOn(cane)); 

	return childrenAreDependent;
}

bool Cane :: hasDependencyOn(GlassColor* glassColor) 
{
	for (unsigned int i = 0; i < this->_casings.size(); ++i) 
	{
		if (this->_casings[i].glassColor == glassColor)
			return true;
	} 

	bool childrenAreDependent = false;
	for (unsigned int i = 0; i < this->_subs.size(); ++i) 
		childrenAreDependent = (childrenAreDependent || this->_subs[i].cane->hasDependencyOn(glassColor)); 

	return childrenAreDependent;
}

void Cane :: setTemplateType(CaneTemplate::Type t)
{
	if (t == this->_type)
		return;

	// if you're switching to a template where count matters, and it's a funky value,
	// set it to something more reasonable
	if (this->_count < 2 && templateHasSubcanes(t))
		this->_count = 7;

	this->_type = t;

	vector<Casing> &casings = this->_casings;
	switch (t) 
	{
		case CaneTemplate::BASE_CIRCLE:
			casings[0].shape = CIRCLE_SHAPE;
			break;
		case CaneTemplate::BASE_SQUARE:
			casings[0].shape = SQUARE_SHAPE;
			this->_twist = 0.0;
			if (casings.size() > 1) 
				casings[0].thickness = MIN(casings[0].thickness, 0.9 * 1 / SQRT_TWO * casings[1].thickness);
			break;
		case CaneTemplate::HORIZONTAL_LINE_CIRCLE:
			casings[0].shape = CIRCLE_SHAPE;
			break;
		case CaneTemplate::HORIZONTAL_LINE_SQUARE:
			casings[0].shape  = CIRCLE_SHAPE;
			break;
		case CaneTemplate::SURROUNDING_CIRCLE:
			casings[0].shape  = CIRCLE_SHAPE;
			break;
		case CaneTemplate::CROSS:
		case CaneTemplate::TRIPOD:
			casings[0].shape  = CIRCLE_SHAPE;
			break;
		case CaneTemplate::SQUARE_OF_SQUARES:
		case CaneTemplate::SQUARE_OF_CIRCLES:
			casings[0].shape = SQUARE_SHAPE;
			this->_twist = 0.0;
			if (casings.size() > 1)
				casings[0].thickness = MIN(casings[0].thickness, 0.9 * 1 / SQRT_TWO * casings[1].thickness);
			break;
		case CaneTemplate::SURROUNDING_SQUARE:
			casings[0].shape = SQUARE_SHAPE;
			if (casings.size() > 1)
				casings[0].thickness = MIN(casings[0].thickness, 0.9 * 1 / SQRT_TWO * casings[1].thickness);
			break;
		case CaneTemplate::CUSTOM:
			// we don't touch anything, who knows what's going on in there
			break;
	}

	resetSubs(true);
}

void Cane :: setCasingColor(GlassColor* gc, unsigned int index) 
{
	if (index >= this->_casings.size())
		return;
	this->_casings[index].glassColor = gc;
}

void Cane :: setOutermostCasingColor(GlassColor* gc) 
{
	int last = this->_casings.size()-1;
	this->_casings[last].glassColor = gc;
}

const GlassColor* Cane :: outermostCasingColor() 
{
	int last = this->_casings.size()-1;
	return this->_casings[last].glassColor;
}

const GlassColor* Cane :: getCasingColor(unsigned int index) 
{
	if (index >= this->_casings.size())
		return NULL;
	return this->_casings[index].glassColor;
}

bool Cane :: hasMinimumCasingCount() 
{
	return (this->_casings.size() < 2);
}	
		
unsigned int Cane :: casingCount() 
{
	return this->_casings.size();
}

enum CaneTemplate::Type Cane :: templateType() const 
{
	return this->_type;
}

unsigned int Cane :: count()
{
	return this->_count;
}

void Cane :: setCount(unsigned int _count)
{
	this->_count = _count;
	resetSubs(false);
}

void Cane :: removeCasing() 
{
	vector<Casing>& casings = this->_casings;

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
	for (unsigned int i = 0; i < this->_subs.size(); ++i)
		this->_subs[i].rescale(casings[0].thickness / oldInnermostCasingThickness);
}


void Cane :: addCasing(enum GeometricShape _shape) 
{
	vector<Casing>& casings = this->_casings;

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
		this->_twist = 0.0;

	// update subpulls by rescaling them according to innermost casing rescaling
	for (unsigned int i = 0; i < this->_subs.size(); ++i) 
		this->_subs[i].rescale(casings[0].thickness / oldInnermostCasingThickness);
}

void Cane :: setCasingThickness(float t, unsigned int index) 
{
	vector<Casing> &casings = this->_casings;	

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
		for (unsigned int i = 0; i < this->_subs.size(); ++i) 
			this->_subs[i].rescale(scaleRatio);
	}
	// otherwise just change the casing
	else
		this->_casings[index].thickness = t;
}

void Cane :: setOutermostCasingShape(enum GeometricShape _shape) 
{
	vector<Casing> &casings = this->_casings;	

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
		if (this->_type == CaneTemplate::BASE_CIRCLE && _shape == SQUARE_SHAPE)
			this->setTemplateType(CaneTemplate::BASE_SQUARE);
		else if (this->_type == CaneTemplate::BASE_SQUARE && _shape == CIRCLE_SHAPE)
			this->setTemplateType(CaneTemplate::BASE_CIRCLE);
	}

	// DO THE CHANGE
	casings[casings.size()-1].shape = _shape;
	if (this->outermostCasingShape() != CIRCLE_SHAPE)
		this->_twist = 0.0;

	resetSubs(false);
}

float Cane :: getCasingThickness(unsigned int index) 
{
	return this->_casings[index].thickness;
}

enum GeometricShape Cane :: outermostCasingShape() 
{
	int last = this->_casings.size()-1;
	return this->_casings[last].shape;
}

enum GeometricShape Cane :: getCasingShape(unsigned int index) 
{
	return this->_casings[index].shape;
}

void Cane :: pushNewSubpull(bool hardReset, vector<SubcaneTemplate>* newSubs,
	enum GeometricShape _shape, Point2D p, float diameter) 
{
	Cane* cane = NULL;

	// if it's not a hard reset and there are still old subcanes to use and the next one matches shape
	// with the shape we want to have, then use it
	if (!hardReset && newSubs->size() < this->_subs.size() 
		&& _shape == this->_subs[newSubs->size()].shape) 
	{
		cane = this->_subs[newSubs->size()].cane;
	}
	else // otherwise just use whichever filler subcane matches the shape
	{
		switch (_shape) 
		{
			case CIRCLE_SHAPE:
				cane = GlobalGlass::circleCane();
				break;
			case SQUARE_SHAPE:
				cane = GlobalGlass::squareCane();
				break;
		}
	}

	newSubs->push_back(SubcaneTemplate(cane, _shape, p, diameter));
}

// resetSubs()

// Description:
// This function is invoked after the template or a template parameter,
// has changed in the cane. The purpose is to recompute the locations 
// and sizes of subcanes, as well as add or remove subcanes if the number 
// of subcanes changed. For instance, changing a template parameter 
// specifying the number of subcanes in a row changes the size and location 
// of subcanes, as well as increasing or decreasing the number of subcanes.
void Cane :: resetSubs(bool hardReset)
{
	Point2D p = make_vector(0.0f, 0.0f);
	float radius = this->_casings[0].thickness;

	vector<SubcaneTemplate> newSubs;
	
	switch (this->_type) 
	{
		case CaneTemplate::BASE_CIRCLE:
		case CaneTemplate::BASE_SQUARE:
			break;
		case CaneTemplate::HORIZONTAL_LINE_CIRCLE:
		{
			for (unsigned int i = 0; i < this->_count; ++i) 
			{
				float littleRadius = (2 * radius / this->_count) / 2;
				p.x = -radius + littleRadius + i * 2 * littleRadius;
				pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, littleRadius * 2.0);
			}
			break;
		}
		case CaneTemplate::HORIZONTAL_LINE_SQUARE:
		{
			radius *= 0.9;
			for (unsigned int i = 0; i < this->_count; ++i) 
			{
				float littleRadius = (2 * radius / this->_count) / 2;
				p.x = -radius + littleRadius + i * 2 * littleRadius;
				pushNewSubpull(hardReset, &newSubs, SQUARE_SHAPE, p, littleRadius * 2.0);
			}
			break;
		}
		case CaneTemplate::SURROUNDING_CIRCLE:
		{
			if (this->_count == 0)
				break;

			unsigned int littleCount = MAX(this->_count-1, 3);
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
		case CaneTemplate::CROSS:
		case CaneTemplate::TRIPOD:
		{
			if (this->_count == 0)
				break;

			unsigned int wings = 3 + static_cast<int>(this->_type == CaneTemplate::CROSS);
			unsigned int sideCount = (this->_count + 1) / wings; 
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
		case CaneTemplate::SQUARE_OF_CIRCLES:
		case CaneTemplate::SQUARE_OF_SQUARES:
		{
			if (this->_count == 0)
				break;

			if (this->_casings[0].shape == CIRCLE_SHAPE)
				radius *= 1 / SQRT_TWO;

			unsigned int sideCount = 0;
			while (sideCount * sideCount < this->_count)
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
					if (this->_type == CaneTemplate::SQUARE_OF_CIRCLES)
						pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, 2 * littleRadius);
					else
						pushNewSubpull(hardReset, &newSubs, SQUARE_SHAPE, p, 2 * littleRadius);
				}
				for (j = s; j <= s; --j) 
				{
					i = s;
					p.x = -radius + littleRadius + 2 * littleRadius * i;
					p.y = -radius + littleRadius + 2 * littleRadius * j;
					if (this->_type == CaneTemplate::SQUARE_OF_CIRCLES)
						pushNewSubpull(hardReset, &newSubs, CIRCLE_SHAPE, p, 2 * littleRadius);
					else
						pushNewSubpull(hardReset, &newSubs, SQUARE_SHAPE, p, 2 * littleRadius);
				}
				
			}
			break;
		}
		case CaneTemplate::SURROUNDING_SQUARE:
		{
			if (this->_count == 0)
				break;

			// (1-8) : 2, (9-12) : 3, (13-16) : 4
			unsigned int sideCount = (MAX(this->_count, 5) + 3) / 4; 

			if (this->_casings[0].shape == CIRCLE_SHAPE)
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
		case CaneTemplate::CUSTOM:
		{
			for (unsigned int i = 0; i < this->_subs.size(); i++)
			{
				p.x = this->_subs[i].location.x * radius;
				p.y = this->_subs[i].location.y * radius;
				// never do a hard reset of custom, because 
				// it's a `soft' template change from a rigid one 
				// to a custom one, so mapping from old cane locations/subcanes
				// to new ones is direct and very natural
				pushNewSubpull(false, &newSubs, this->_subs[i].shape, this->_subs[i].location, 
					this->_subs[i].diameter);
			}
			break;
		}
	}

	this->_subs = newSubs;
}

SubcaneTemplate Cane :: getSubcaneTemplate(unsigned int index)
{
	return this->_subs[index];
}

void Cane :: setSubcaneTemplate(SubcaneTemplate t, unsigned int index)
{
	this->_subs[index] = t;
}

void Cane :: addSubcaneTemplate(SubcaneTemplate t)
{
	this->_subs.push_back(t);
}

void Cane :: removeSubcaneTemplate(unsigned int index)
{
	this->_subs.erase(this->_subs.begin() + index);
}

unsigned int Cane :: subpullCount()
{
	return this->_subs.size();
}

float Cane :: twist()
{
	return this->_twist;
}

void Cane :: setTwist(float t)
{
	this->_twist = t;
}

float* Cane :: twistPtr()
{
	return &(this->_twist);
}

Cane* Cane :: copy() const 
{
	Cane* c = new Cane(this->_type);
	c->_type = this->_type;
	c->_casings = this->_casings;
	c->_count = this->_count;
	c->_twist = this->_twist;
	c->_subs = this->_subs;
	return c;
}

Cane *deep_copy(const Cane *_cane) 
{
	unordered_map<const Cane*, Cane*> copies;
	Cane *cane = _cane->copy();
	copies.insert(make_pair(_cane, cane));

	vector<Cane*> to_update;
	to_update.push_back(cane);
	//update sub-templates to point to copies as well:
	while (!to_update.empty()) 
	{
		Cane *t = to_update.back();
		to_update.pop_back();
		for (unsigned int i = 0; i < t->subpullCount(); ++i)
		{
			SubcaneTemplate s = t->getSubcaneTemplate(i);
			unordered_map<const Cane*, Cane*>::iterator f = copies.find(s.cane);
			if (f == copies.end()) 
			{
				f = copies.insert(make_pair(s.cane, s.cane->copy())).first;
				to_update.push_back(f->second);
			}
			s.cane = f->second;
			t->setSubcaneTemplate(s, i);
		}
	}
	return cane;
}

void deep_delete(Cane *cane) 
{
	//Because canes don't delete their children (which is right):
	unordered_set<Cane*> marked;
	vector<Cane*> to_delete;
	to_delete.push_back(cane);
	while (!to_delete.empty()) 
	{
		Cane *t = to_delete.back();
		to_delete.pop_back();
		for (unsigned int i = 0; i < t->subpullCount(); ++i)
		{
			SubcaneTemplate s = t->getSubcaneTemplate(i);
			if (marked.insert(s.cane).second) 
				to_delete.push_back(s.cane);
			s.cane = NULL;
			t->setSubcaneTemplate(s, i);
		}
		delete t;
	}
}

