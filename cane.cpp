
#include <QDebug>

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
	this->twist_ = 0;

	// initialize casings and subcanes to something simple
	this->casings_.push_back(Casing(1.0, CIRCLE_SHAPE, GlobalGlass::color()));
	this->addCasingDependency(GlobalGlass::color());
	this->casings_[0].shape = CIRCLE_SHAPE;
	this->count_ = 0;
	this->type_ = CaneTemplate::BASE_CIRCLE;

	// now initialize for real
	setTemplateType(t);
}

bool Cane :: hasDependencyOn(GlassColor* glassColor) const
{
	for (unsigned int i = 0; i < this->casings_.size(); ++i)
	{
		if (this->casings_[i].glassColor == glassColor)
			return true;
	}

	bool childrenAreDependent = false;
	for (unsigned int i = 0; i < this->subcanes_.size(); ++i)
		childrenAreDependent = (childrenAreDependent || this->subcanes_[i].cane->hasDependencyOn(glassColor));

	return childrenAreDependent;
}

bool Cane :: hasDependencyOn(Cane* cane) const
{
	if (this == cane)
		return true;

	bool childrenAreDependent = false;
	for (unsigned int i = 0; i < this->subcanes_.size(); ++i)
		childrenAreDependent = (childrenAreDependent || this->subcanes_[i].cane->hasDependencyOn(cane));

	return childrenAreDependent;
}

unsigned int Cane :: casingDependencyOccurrances(GlassColor* glassColor)
{
	int occurrances = 0;
	for (unsigned int i = 0; i < this->casings_.size(); ++i)
		occurrances += (this->casings_[i].glassColor == glassColor);
	return occurrances;
}

void Cane :: addCasingDependency(GlassColor* glassColor)
{
	if (casingDependencyOccurrances(glassColor) == 1)
		connect(glassColor, SIGNAL(modified()), this, SLOT(dependencyModified()));
}

void Cane :: removeCasingDependency(GlassColor* glassColor)
{
	if (casingDependencyOccurrances(glassColor) == 0)
		disconnect(glassColor, SIGNAL(modified()), this, SLOT(dependencyModified()));
}

unsigned int Cane :: subcaneDependencyOccurrances(Cane* cane)
{
	int occurrances = 0;
	for (unsigned int i = 0; i < this->subcanes_.size(); ++i)
		occurrances += (this->subcanes_[i].cane == cane);
	return occurrances;
}

void Cane :: addSubcaneDependency(Cane* cane)
{
	if (subcaneDependencyOccurrances(cane) == 1)
		connect(cane, SIGNAL(modified()), this, SLOT(dependencyModified()));
}

void Cane :: removeSubcaneDependency(Cane* cane)
{
	if (subcaneDependencyOccurrances(cane) == 0)
		disconnect(cane, SIGNAL(modified()), this, SLOT(dependencyModified()));
}

void Cane :: dependencyModified()
{
	emit modified();
}

void Cane :: setTemplateType(CaneTemplate::Type type)
{
	if (type == this->type_)
		return;

	// if you're switching to a template where count matters, and it's a funky value,
	// set it to something more reasonable
	if (this->count_ < 2 && templateHasSubcanes(type))
		this->count_ = 7;

	this->type_ = type;

	vector<Casing> &casings = this->casings_;
	switch (type) 
	{
		case CaneTemplate::BASE_CIRCLE:
			casings[0].shape = CIRCLE_SHAPE;
			break;
		case CaneTemplate::BASE_SQUARE:
			casings[0].shape = SQUARE_SHAPE;
			this->twist_ = 0.0;
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
			this->twist_ = 0.0;
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
	resetSubcanes(true); // emits modified()
}

void Cane :: setCasingColor(GlassColor* glassColor, unsigned int index) 
{
	if (index >= this->casings_.size())
		return;
	GlassColor* old = this->casings_[index].glassColor;
	this->casings_[index].glassColor = NULL;
	this->removeCasingDependency(old);
	this->casings_[index].glassColor = glassColor;
	this->addCasingDependency(glassColor);
	emit modified();
}

void Cane :: setOutermostCasingColor(GlassColor* gc) 
{
	int last = this->casings_.size()-1;
	setCasingColor(gc, last);
}

const GlassColor* Cane :: casingColor(unsigned int index) const 
{
	if (index >= this->casings_.size())
		return NULL;
	return this->casings_[index].glassColor;
}

const GlassColor* Cane :: outermostCasingColor() const
{
	int last = this->casings_.size()-1;
	return casingColor(last);
}

bool Cane :: hasMinimumCasingCount() const 
{
	return (this->casings_.size() < 2);
}	
		
unsigned int Cane :: casingCount() const 
{
	return this->casings_.size();
}

enum CaneTemplate::Type Cane :: templateType() const 
{
	return this->type_;
}

unsigned int Cane :: count() const
{
	return this->count_;
}

void Cane :: setCount(unsigned int count_)
{
	this->count_ = count_;
	resetSubcanes(false);
}

void Cane :: removeCasing() 
{
	vector<Casing>& casings = this->casings_;

	int count = casings.size();
	if (count < 2) 
		return;

	float oldInnermostCasingThickness = casings[0].thickness;

	// "puff out" the casing thicknesses so second outermost now has radius of 
	// previous outermost one
	float diff = 1.0 - casings[count-2].thickness;
	Casing removedCasing = casings.back();
	casings.pop_back();
	this->removeCasingDependency(removedCasing.glassColor);
	casings[count-2].thickness = 1.0;
	for (unsigned int i = 0; i < casings.size()-1; ++i) 
	{
		if (casings[i].shape == casings[i+1].shape || casings[i].shape == CIRCLE_SHAPE)
			casings[i].thickness += diff;
		else
			casings[i].thickness += diff/SQRT_TWO;
	}

	// rescale subcanes
	for (unsigned int i = 0; i < this->subcanes_.size(); ++i)
		this->subcanes_[i].rescale(casings[0].thickness / oldInnermostCasingThickness);

	emit modified();
}


void Cane :: addCasing(enum GeometricShape _shape) 
{
	vector<Casing>& casings = this->casings_;

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
	this->addCasingDependency(GlobalGlass::color());
	if (this->outermostCasingShape() != CIRCLE_SHAPE)
		this->twist_ = 0.0;

	// update subcanes by rescaling them according to innermost casing rescaling
	for (unsigned int i = 0; i < this->subcanes_.size(); ++i) 
		this->subcanes_[i].rescale(casings[0].thickness / oldInnermostCasingThickness);

	emit modified();
}

void Cane :: setCasingThickness(float t, unsigned int index) 
{
	vector<Casing> &casings = this->casings_;	

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
		for (unsigned int i = 0; i < this->subcanes_.size(); ++i) 
			this->subcanes_[i].rescale(scaleRatio);
	}
	// otherwise just change the casing
	else
		this->casings_[index].thickness = t;
	
	emit modified();
}

void Cane :: setOutermostCasingShape(enum GeometricShape _shape) 
{
	vector<Casing> &casings = this->casings_;	

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
		if (this->type_ == CaneTemplate::BASE_CIRCLE && _shape == SQUARE_SHAPE)
			this->setTemplateType(CaneTemplate::BASE_SQUARE);
		else if (this->type_ == CaneTemplate::BASE_SQUARE && _shape == CIRCLE_SHAPE)
			this->setTemplateType(CaneTemplate::BASE_CIRCLE);
	}

	// DO THE CHANGE
	casings[casings.size()-1].shape = _shape;
	if (this->outermostCasingShape() != CIRCLE_SHAPE)
		this->twist_ = 0.0;

	resetSubcanes(false); // calls emit modified
}

float Cane :: casingThickness(unsigned int index) const 
{
	return this->casings_[index].thickness;
}

enum GeometricShape Cane :: outermostCasingShape() const 
{
	int last = this->casings_.size()-1;
	return casingShape(last);
}

enum GeometricShape Cane :: casingShape(unsigned int index) const
{
	return this->casings_[index].shape;
}

void Cane :: pushNewSubcane(bool hardReset, vector<SubcaneTemplate>* newSubcanes,
	enum GeometricShape _shape, Point2D p, float diameter) 
{
	Cane* cane = NULL;

	// if it's not a hard reset and there are still old subcanes to use and the next one matches shape
	// with the shape we want to have, then use it
	if (!hardReset && newSubcanes->size() < this->subcanes_.size() 
		&& _shape == this->subcanes_[newSubcanes->size()].shape) 
	{
		cane = this->subcanes_[newSubcanes->size()].cane;
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

	newSubcanes->push_back(SubcaneTemplate(cane, _shape, p, diameter));
}

// resetSubcanes()

// Description:
// This function is invoked after the template or a template parameter,
// has changed in the cane. The purpose is to recompute the locations 
// and sizes of subcanes, as well as add or remove subcanes if the number 
// of subcanes changed. For instance, changing a template parameter 
// specifying the number of subcanes in a row changes the size and location 
// of subcanes, as well as increasing or decreasing the number of subcanes.
void Cane :: resetSubcanes(bool hardReset)
{
	Point2D p = make_vector(0.0f, 0.0f);
	float radius = this->casings_[0].thickness;

	vector<SubcaneTemplate> newSubcanes;
	
	switch (this->type_) 
	{
		case CaneTemplate::BASE_CIRCLE:
		case CaneTemplate::BASE_SQUARE:
			break;
		case CaneTemplate::HORIZONTAL_LINE_CIRCLE:
		{
			for (unsigned int i = 0; i < this->count_; ++i) 
			{
				float littleRadius = (2 * radius / this->count_) / 2;
				p.x = -radius + littleRadius + i * 2 * littleRadius;
				pushNewSubcane(hardReset, &newSubcanes, CIRCLE_SHAPE, p, littleRadius * 2.0);
			}
			break;
		}
		case CaneTemplate::HORIZONTAL_LINE_SQUARE:
		{
			radius *= 0.9;
			for (unsigned int i = 0; i < this->count_; ++i) 
			{
				float littleRadius = (2 * radius / this->count_) / 2;
				p.x = -radius + littleRadius + i * 2 * littleRadius;
				pushNewSubcane(hardReset, &newSubcanes, SQUARE_SHAPE, p, littleRadius * 2.0);
			}
			break;
		}
		case CaneTemplate::SURROUNDING_CIRCLE:
		{
			if (this->count_ == 0)
				break;

			unsigned int littleCount = MAX(this->count_-1, 3);
			float theta = TWO_PI / littleCount;
			float k = sin(theta/2) / (1 + sin(theta/2));
			pushNewSubcane(hardReset, &newSubcanes, CIRCLE_SHAPE, p, (1 - 2 * k) * 2 * radius);
			p.x = p.y = 0.0;
			for (unsigned int i = 0; i < littleCount; ++i) 
			{
				p.x = (1.0 - k) * radius * cos(TWO_PI / littleCount * i);
				p.y = (1.0 - k) * radius * sin(TWO_PI / littleCount * i);
				pushNewSubcane(hardReset, &newSubcanes, CIRCLE_SHAPE, p, 2 * k * radius);
			}
			break;
		}
		case CaneTemplate::CROSS:
		case CaneTemplate::TRIPOD:
		{
			if (this->count_ == 0)
				break;

			unsigned int wings = 3 + static_cast<int>(this->type_ == CaneTemplate::CROSS);
			unsigned int sideCount = (this->count_ + 1) / wings; 
			float littleRadius = (radius / (sideCount + 0.5)) / 2.0;
	
			p.x = p.y = 0.0;
			pushNewSubcane(hardReset, &newSubcanes, CIRCLE_SHAPE, p, littleRadius * 2.0);
			for (unsigned int i = 0; i < sideCount; ++i) 
			{
				for (unsigned int theta = 0; theta < wings; ++theta) 
				{
					p.x = (littleRadius * 2 * (i+1)) * cos(TWO_PI / wings * theta);
					p.y = (littleRadius * 2 * (i+1)) * sin(TWO_PI / wings * theta);
					pushNewSubcane(hardReset, &newSubcanes, CIRCLE_SHAPE, p, littleRadius * 2);
				}
			}
			break;
		}
		case CaneTemplate::SQUARE_OF_CIRCLES:
		case CaneTemplate::SQUARE_OF_SQUARES:
		{
			if (this->count_ == 0)
				break;

			if (this->casings_[0].shape == CIRCLE_SHAPE)
				radius *= 1 / SQRT_TWO;

			unsigned int sideCount = 0;
			while (sideCount * sideCount < this->count_)
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
					if (this->type_ == CaneTemplate::SQUARE_OF_CIRCLES)
						pushNewSubcane(hardReset, &newSubcanes, CIRCLE_SHAPE, p, 2 * littleRadius);
					else
						pushNewSubcane(hardReset, &newSubcanes, SQUARE_SHAPE, p, 2 * littleRadius);
				}
				for (j = s; j <= s; --j) 
				{
					i = s;
					p.x = -radius + littleRadius + 2 * littleRadius * i;
					p.y = -radius + littleRadius + 2 * littleRadius * j;
					if (this->type_ == CaneTemplate::SQUARE_OF_CIRCLES)
						pushNewSubcane(hardReset, &newSubcanes, CIRCLE_SHAPE, p, 2 * littleRadius);
					else
						pushNewSubcane(hardReset, &newSubcanes, SQUARE_SHAPE, p, 2 * littleRadius);
				}
				
			}
			break;
		}
		case CaneTemplate::SURROUNDING_SQUARE:
		{
			if (this->count_ == 0)
				break;

			// (1-8) : 2, (9-12) : 3, (13-16) : 4
			unsigned int sideCount = (MAX(this->count_, 5) + 3) / 4; 

			if (this->casings_[0].shape == CIRCLE_SHAPE)
				radius *= 1 / SQRT_TWO;

			float littleRadius = radius / (sideCount + 1);

			p.x = p.y = 0.0;
			pushNewSubcane(hardReset, &newSubcanes, SQUARE_SHAPE, p, 
				2 * littleRadius * (sideCount-1));
			for (unsigned int i = 0; i < sideCount; ++i)
			{
				p.x = -2 * littleRadius * sideCount / 2.0 + 2 * littleRadius * i;
				p.y = -2 * littleRadius * sideCount / 2.0;
				pushNewSubcane(hardReset, &newSubcanes, CIRCLE_SHAPE, p, 2 * littleRadius);
			}
			for (unsigned int j = 0; j < sideCount; ++j)
			{
				p.x = -2 * littleRadius * sideCount / 2.0 + 2 * littleRadius * sideCount; 
				p.y = -2 * littleRadius * sideCount / 2.0 + 2 * littleRadius * j;
				pushNewSubcane(hardReset, &newSubcanes, CIRCLE_SHAPE, p, 2 * littleRadius);
			}
			for (unsigned int i = sideCount; i >= 1; --i) 
			{
				p.x = -2 * littleRadius * sideCount / 2.0 + 2 * littleRadius * i;
				p.y = -2 * littleRadius * sideCount / 2.0 + 2 * littleRadius * sideCount;
				pushNewSubcane(hardReset, &newSubcanes, CIRCLE_SHAPE, p, 2 * littleRadius);
			}
			for (unsigned int j = sideCount; j >= 1; --j) 
			{
				p.x = -2 * littleRadius * sideCount / 2.0;
				p.y = -2 * littleRadius * sideCount / 2.0 + 2 * littleRadius * j;
				pushNewSubcane(hardReset, &newSubcanes, CIRCLE_SHAPE, p, 2 * littleRadius);
			}
			break;
		}
		case CaneTemplate::CUSTOM:
		{
			for (unsigned int i = 0; i < this->subcanes_.size(); i++)
			{
				p.x = this->subcanes_[i].location.x * radius;
				p.y = this->subcanes_[i].location.y * radius;
				// never do a hard reset of custom, because 
				// it's a `soft' template change from a rigid one 
				// to a custom one, so mapping from old cane locations/subcanes
				// to new ones is direct and very natural
				pushNewSubcane(false, &newSubcanes, this->subcanes_[i].shape, this->subcanes_[i].location, 
					this->subcanes_[i].diameter);
			}
			break;
		}
	}

	while (this->subcanes_.size() > 0)
	{
		SubcaneTemplate lastSubcane = this->subcanes_.back();
		this->subcanes_.pop_back();
		this->removeSubcaneDependency(lastSubcane.cane);
	}
	while (newSubcanes.size() > 0)
	{
		SubcaneTemplate lastSubcane = newSubcanes.back();
		newSubcanes.pop_back();
		this->subcanes_.push_back(lastSubcane);
		this->addSubcaneDependency(lastSubcane.cane);
	}

	emit modified();
}

SubcaneTemplate Cane :: subcaneTemplate(unsigned int index) const
{
	return this->subcanes_[index];
}

void Cane :: setSubcaneTemplate(SubcaneTemplate subcane, unsigned int index)
{
	Cane* old = this->subcanes_[index].cane;
	this->subcanes_[index].cane = NULL;
	this->removeSubcaneDependency(old);
	this->subcanes_[index] = subcane;
	this->addSubcaneDependency(subcane.cane);
	emit modified();
}

void Cane :: addSubcaneTemplate(SubcaneTemplate t)
{
	this->subcanes_.push_back(t);
	this->addSubcaneDependency(t.cane);
	emit modified();
}

void Cane :: removeSubcaneTemplate(unsigned int index)
{
	SubcaneTemplate old = this->subcanes_[index];
	this->subcanes_.erase(this->subcanes_.begin() + index);
	this->removeSubcaneDependency(old.cane);
	emit modified();
}

unsigned int Cane :: subcaneCount() const
{
	return this->subcanes_.size();
}

float Cane :: twist() const
{
	return this->twist_;
}

void Cane :: setTwist(float t)
{
	this->twist_ = t;
	emit modified();
}

Cane* Cane :: copy() const 
{
	Cane* c = new Cane(this->type_);
	c->type_ = this->type_;
	while (c->casings_.size() > 0)
	{
		Casing lastCasing = c->casings_.back();
		c->casings_.pop_back();
		c->removeCasingDependency(lastCasing.glassColor);
	}
	for (unsigned int i = 0; i < this->casings_.size(); ++i)
	{
		c->casings_.push_back(this->casings_[i]);
		c->addCasingDependency(this->casings_[i].glassColor);	
	}
	c->count_ = this->count_;
	c->twist_ = this->twist_;
	while (c->subcanes_.size() > 0)
	{
		SubcaneTemplate lastSubcane = c->subcanes_.back();
		c->subcanes_.pop_back();
		c->removeSubcaneDependency(lastSubcane.cane);
	}
	for (unsigned int i = 0; i < this->subcanes_.size(); ++i)
	{
		c->subcanes_.push_back(this->subcanes_[i]);
		c->addSubcaneDependency(this->subcanes_[i].cane);	
	}

	return c;
}

void Cane :: set(Cane* c)
{
	this->type_ = c->type_;
	while (this->casings_.size() > 0)
	{
		Casing lastCasing = this->casings_.back();
		this->casings_.pop_back();
		this->removeCasingDependency(lastCasing.glassColor);
	}
	for (unsigned int i = 0; i < c->casings_.size(); ++i)
	{
		this->casings_.push_back(c->casings_[i]);
		this->addCasingDependency(c->casings_[i].glassColor);	
	}
	this->count_ = c->count_;
	this->twist_ = c->twist_;
	while (this->subcanes_.size() > 0)
	{
		SubcaneTemplate lastSubcane = this->subcanes_.back();
		this->subcanes_.pop_back();
		this->removeSubcaneDependency(lastSubcane.cane);
	}
	for (unsigned int i = 0; i < c->subcanes_.size(); ++i)
	{
		this->subcanes_.push_back(c->subcanes_[i]);
		this->addSubcaneDependency(c->subcanes_[i].cane);	
	}
	emit modified();
}

// WARNING: Does not create any modification signals in the copy.
// To be used as a reference only, and not to be modified.
Cane * deep_copy(const Cane *_cane) 
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
		for (unsigned int i = 0; i < t->subcaneCount(); ++i)
		{
			SubcaneTemplate s = t->subcaneTemplate(i);
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

// WARNING: Does not remove any modification signals found in the copy.
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
		for (unsigned int i = 0; i < t->subcaneCount(); ++i)
		{
			Cane* subcane = t->subcaneTemplate(i).cane;
			if (marked.insert(subcane).second) 
				to_delete.push_back(subcane);
		}
		delete t;
	}
}

