
#include "piece.h"
#include "cane.h"
#include "constants.h"
#include "globalglass.h"

Piece :: Piece(enum PieceTemplate::Type pieceType, enum PickupTemplate::Type pickupType)
{
	this->twist_ = 0.0;
	setPieceTemplateType(pieceType, true);

	this->casingGlassColor_ = GlobalGlass::color();
	this->underlayGlassColor_ = GlobalGlass::color();
	this->overlayGlassColor_ = GlobalGlass::color();
	addCasingDependency(GlobalGlass::color());
	setPickupTemplateType(pickupType, true);
}

unsigned int Piece :: casingDependencyOccurrances(GlassColor* glassColor)
{
	int occurrances = 0;

	if (this->casingGlassColor_ == glassColor)
		++occurrances;
	if (this->underlayGlassColor_ == glassColor)
		++occurrances;
	if (this->overlayGlassColor_ == glassColor)
		++occurrances;

	return occurrances;
}

void Piece :: addCasingDependency(GlassColor* glassColor)
{
	if (casingDependencyOccurrances(glassColor) == 1)
		connect(glassColor, SIGNAL(modified()), this, SLOT(dependencyModified()));
}

void Piece :: removeCasingDependency(GlassColor* glassColor)
{
	if (casingDependencyOccurrances(glassColor) == 0)
		disconnect(glassColor, SIGNAL(modified()), this, SLOT(dependencyModified()));
}

unsigned int Piece :: subcaneDependencyOccurrances(Cane* cane)
{
	int occurrances = 0;
	for (unsigned int i = 0; i < this->subcanes_.size(); ++i)
		occurrances += (this->subcanes_[i].cane == cane);
	return occurrances;
}

void Piece :: addSubcaneDependency(Cane* cane)
{
	if (subcaneDependencyOccurrances(cane) == 1)
		connect(cane, SIGNAL(modified()), this, SLOT(dependencyModified()));
}

void Piece :: removeSubcaneDependency(Cane* cane)
{
	if (subcaneDependencyOccurrances(cane) == 0)
		disconnect(cane, SIGNAL(modified()), this, SLOT(dependencyModified()));
}

void Piece::setSpline(Spline s)
{
	this->spline_ = s;
	emit modified();
}

Spline Piece::spline() const
{
	return this->spline_;
}

void Piece :: setTwist(float t)
{
	this->twist_ = t;
	emit modified();
}

float Piece :: twist() const
{
	return this->twist_;
}

void Piece :: dependencyModified()
{
	emit modified();
}

bool Piece :: hasDependencyOn(Cane* cane) const
{
	bool pickupsDependOn = false;

	for (unsigned int i = 0; i < this->subpickupCount(); ++i)
	{
		if (this->subpickupTemplate(i).cane->hasDependencyOn(cane))
		{
			pickupsDependOn = true;
			break;
		}
	}

	return pickupsDependOn;
}

bool Piece :: hasDependencyOn(GlassColor* glassColor) const
{
	bool pickupsDependOn = false;

	for (unsigned int i = 0; i < this->subpickupCount(); ++i)
	{
		if (this->subpickupTemplate(i).cane->hasDependencyOn(glassColor))
		{
			pickupsDependOn = true;		
			break;
		}
	}
	
	if (overlayGlassColor_ == glassColor || underlayGlassColor_ == glassColor)
		pickupsDependOn = true;

	return pickupsDependOn;
}

Piece* Piece :: copy() const
{
	Piece* p = new Piece(this->pieceType_, this->pickupType_);

	p->twist_ = this->twist_;
	p->pieceType_ = this->pieceType_;
	p->spline_ = this->spline_;
	GlassColor* old = p->overlayGlassColor_;
	p->overlayGlassColor_ = NULL;
	p->removeCasingDependency(old);
	p->overlayGlassColor_ = this->overlayGlassColor_;
	p->addCasingDependency(p->overlayGlassColor_);
	old = p->underlayGlassColor_;
	p->underlayGlassColor_ = NULL;
	p->removeCasingDependency(old);
	p->underlayGlassColor_ = this->underlayGlassColor_;
	p->addCasingDependency(p->underlayGlassColor_);
	old = p->casingGlassColor_;
	p->casingGlassColor_ = NULL;
	p->removeCasingDependency(old);
	p->casingGlassColor_ = this->casingGlassColor_;
	p->addCasingDependency(p->casingGlassColor_);
	while (p->subcanes_.size() > 0)
	{
		SubpickupTemplate lastSubcane = p->subcanes_.back();
		p->subcanes_.pop_back();
		p->removeSubcaneDependency(lastSubcane.cane);
	}
	for (unsigned int i = 0; i < this->subcanes_.size(); ++i)
	{
		p->subcanes_.push_back(this->subcanes_[i]);
		p->addSubcaneDependency(this->subcanes_[i].cane);
	}
	p->pickupParameters_ = this->pickupParameters_;
	p->pickupType_ = this->pickupType_;

	return p;
}

void Piece :: set(Piece* p)
{
	this->twist_ = p->twist_;
	this->pieceType_ = p->pieceType_;
	this->spline_ = p->spline_;	
	GlassColor* old = this->overlayGlassColor_;
	this->overlayGlassColor_ = NULL;
	this->removeCasingDependency(old);
	this->overlayGlassColor_ = p->overlayGlassColor_;
	old = this->underlayGlassColor_;
	this->underlayGlassColor_ = NULL;
	this->removeCasingDependency(old);
	this->underlayGlassColor_ = p->underlayGlassColor_;
	old = this->casingGlassColor_;
	this->casingGlassColor_ = NULL;
	this->removeCasingDependency(old);
	this->casingGlassColor_ = p->casingGlassColor_;
	while (this->subcanes_.size() > 0)
	{
		SubpickupTemplate subcane = this->subcanes_.back();
		this->subcanes_.pop_back();
		this->removeSubcaneDependency(subcane.cane);	
	}	
	for (unsigned int i = 0; i < p->subcanes_.size(); ++i)
	{
		this->subcanes_.push_back(p->subcanes_[i]);
		this->addSubcaneDependency(p->subcanes_[i].cane);
	}
	this->pickupParameters_ = p->pickupParameters_;
	this->pickupType_ = p->pickupType_;
	emit modified();
}

void Piece :: setPieceTemplateType(enum PieceTemplate::Type type, bool force)
{
	if (!force && this->pieceType_ == type)
		return;

	Spline &spline = this->spline_;

	this->pieceType_ = type;
	switch (this->pieceType_)
	{
		case PieceTemplate::TUMBLER:
			while (spline.controlPoints().size() < 4)
				spline.addPoint(Point2D(make_vector(0.0f, 0.0f)));
			while (spline.controlPoints().size() > 4)
				spline.removePoint();
			spline.set(0, Point2D(make_vector(0.0f, -4.24f)));
			spline.set(1, Point2D(make_vector(4.87f, -4.6f)));
			spline.set(2, Point2D(make_vector(3.52f, -0.32f)));
			spline.set(3, Point2D(make_vector(3.7f, 4.77f)));
			break;
		case PieceTemplate::VASE:
			while (spline.controlPoints().size() < 5)
				spline.addPoint(Point2D(make_vector(0.0f, 0.0f)));
			while (spline.controlPoints().size() > 5)
				spline.removePoint();
			spline.set(0, Point2D(make_vector(0.0f, -5.68f)));
			spline.set(1, Point2D(make_vector(3.57f, -5.62f)));
			spline.set(2, Point2D(make_vector(3.54f, -0.02f)));
			spline.set(3, Point2D(make_vector(-0.44f, 1.90f)));
			spline.set(4, Point2D(make_vector(2.22f, 5.13f)));
			break;
		case PieceTemplate::BOWL:
			while (spline.controlPoints().size() < 4)
				spline.addPoint(Point2D(make_vector(0.0f, 0.0f)));
			while (spline.controlPoints().size() > 4)
				spline.removePoint();
			spline.set(0, Point2D(make_vector(0.0f, -2.45f)));
			spline.set(1, Point2D(make_vector(3.31f, -2.41f)));
			spline.set(2, Point2D(make_vector(4.84f, 0.02f)));
			spline.set(3, Point2D(make_vector(4.88f, 2.45f)));
			break;
		case PieceTemplate::POT:
			while (spline.controlPoints().size() < 4)
				spline.addPoint(Point2D(make_vector(0.0f, 0.0f)));
			while (spline.controlPoints().size() > 4)
				spline.removePoint();
			spline.set(0, Point2D(make_vector(0.0f, -4.94f)));
			spline.set(1, Point2D(make_vector(4.27f, -5.17f)));
			spline.set(2, Point2D(make_vector(4.47f, 1.29f)));
			spline.set(3, Point2D(make_vector(1.59f, 4.86f)));
			break;
		case PieceTemplate::PLATE:
			while (spline.controlPoints().size() < 3)
				spline.addPoint(Point2D(make_vector(0.0f, 0.0f)));
			while (spline.controlPoints().size() > 3)
				spline.removePoint();
			spline.set(0, Point2D(make_vector(0.0f, -0.3f)));
			spline.set(1, Point2D(make_vector(4.5f, -0.3f)));
			spline.set(2, Point2D(make_vector(5.0f, 0.7f)));
			break;
		case PieceTemplate::FISHTRAP:
			while (spline.controlPoints().size() < 5)
				spline.addPoint(Point2D(make_vector(0.0f, 0.0f)));
			while (spline.controlPoints().size() > 5)
				spline.removePoint();
			spline.set(0, Point2D(make_vector(0.0f, -4.24f)));
			spline.set(1, Point2D(make_vector(7.1f, -5.37f)));
			spline.set(2, Point2D(make_vector(6.64f, 7.78f)));
			spline.set(3, Point2D(make_vector(-1.34f, 4.64f)));
			spline.set(4, Point2D(make_vector(1.65f, -2.11f)));
			break;
		case PieceTemplate::CUSTOM:
			break;
	}
	emit modified();
}

enum PieceTemplate::Type Piece :: pieceTemplateType() const
{
	return this->pieceType_;
}

SubpickupTemplate Piece :: subpickupTemplate(unsigned int index) const
{
	return this->subcanes_[index];
}

void Piece :: setSubpickupTemplate(SubpickupTemplate subcane, unsigned int index)
{
	Cane* old = this->subcanes_[index].cane;
	this->subcanes_[index].cane = NULL;
	this->removeSubcaneDependency(old);
	this->subcanes_[index] = subcane;
	this->addSubcaneDependency(subcane.cane);	
	emit modified();
}

unsigned int Piece :: subpickupCount() const
{
	return this->subcanes_.size();
}

GlassColor* Piece :: overlayGlassColor() const
{
	return this->overlayGlassColor_;
}

GlassColor* Piece :: underlayGlassColor() const
{
	return this->underlayGlassColor_;
}

GlassColor* Piece :: casingGlassColor() const
{
	return this->casingGlassColor_;
}

void Piece :: setOverlayGlassColor(GlassColor* glassColor)
{
	GlassColor* old = this->overlayGlassColor_;
	this->overlayGlassColor_ = NULL;
	this->removeCasingDependency(old);
	this->overlayGlassColor_ = glassColor;
	this->addCasingDependency(old);
	emit modified();
}

void Piece :: setUnderlayGlassColor(GlassColor* glassColor)
{
	GlassColor* old = this->underlayGlassColor_;
	this->underlayGlassColor_ = NULL;
	this->removeCasingDependency(old);
	this->underlayGlassColor_ = glassColor;
	this->addCasingDependency(old);
	emit modified();
}

void Piece :: setCasingGlassColor(GlassColor* glassColor)
{
	GlassColor* old = this->casingGlassColor_;
	this->casingGlassColor_ = NULL;
	this->removeCasingDependency(old);
	this->casingGlassColor_ = glassColor;
	this->addCasingDependency(old);
	emit modified();
}

void Piece :: pushNewSubcane(vector<SubpickupTemplate>* newSubcanes,
	Point3D location, enum PickupCaneOrientation ori, float length, float width, enum GeometricShape shape)
{
	if (newSubcanes->size() < this->subcanes_.size())
	{
		newSubcanes->push_back(SubpickupTemplate(this->subcanes_[newSubcanes->size()].cane,
			location, ori, length, width, shape));
	}
	else // you've run out of existing subcanes copy from
	{
		newSubcanes->push_back(SubpickupTemplate(GlobalGlass::circleCane(),
			location, ori, length, width, shape));
	}
}

void Piece :: updateSubcanes() 
{
	vector<SubpickupTemplate> newSubcanes;

	vector<TemplateParameter> &parameters = this->pickupParameters_;

	Point3D p;
	float width, length;
	switch (this->pickupType_) 
	{
		case PickupTemplate::MURRINE:
			width = 2.0 / MAX(parameters[0].value, 1);
			length = parameters[1].value*0.005 + 0.005;
			for (int r = 0; r < parameters[0].value; ++r)
			{
				for (int c = 0; c < parameters[0].value; ++c)
				{
					p.x = -1.0 + width / 2 + width * r;
					p.y = -1.0 + width / 2 + width * c;
					p.z = -length/2;
					pushNewSubcane(&newSubcanes, p, MURRINE_PICKUP_CANE_ORIENTATION, 
						length, width-0.0001, SQUARE_SHAPE);
				}
			}
			break;
		case PickupTemplate::MURRINE_COLUMN:
			width = 2.0 / MAX(parameters[0].value, 1);
			for (int i = 0; i < parameters[0].value-1; ++i) {
				p.x = 1.0 - width / 2;
				p.y = -1.0 + width / 2 + width * (parameters[0].value - 1 - i);
				p.z = -width/2;
				pushNewSubcane(&newSubcanes, p, MURRINE_PICKUP_CANE_ORIENTATION, width, width-0.0001, SQUARE_SHAPE);
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				p.z = 0.0;
				pushNewSubcane(&newSubcanes, p, VERTICAL_PICKUP_CANE_ORIENTATION, 2.0, width-0.0001, CIRCLE_SHAPE);
			}
			p.x = 1.0 - width / 2;
			p.y = -1.0 + width / 2;
			p.z = -width/2;
			pushNewSubcane(&newSubcanes, p, MURRINE_PICKUP_CANE_ORIENTATION, width, width-0.0001, SQUARE_SHAPE);
			break;
		case PickupTemplate::MURRINE_ROW:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameters[0].value, 1);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				p.z = 0.0;
				pushNewSubcane(&newSubcanes, p, VERTICAL_PICKUP_CANE_ORIENTATION, 1.0 - width/2, 
					width-0.0001, CIRCLE_SHAPE);
				p.y = 0.0;
				p.x = -1.0 + width / 2 + width * i;
				p.z = -width/2;
				pushNewSubcane(&newSubcanes, p, MURRINE_PICKUP_CANE_ORIENTATION, width, 
					width-0.0001, SQUARE_SHAPE);
				p.x = -1.0 + width / 2 + width * i;
				p.y = width/2;
				p.z = 0.0;
				pushNewSubcane(&newSubcanes, p, VERTICAL_PICKUP_CANE_ORIENTATION, 1.0 - width/2, 
					width-0.0001, CIRCLE_SHAPE);
			}
			break;
		case PickupTemplate::RETICELLO_VERTICAL_HORIZONTAL:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameters[0].value, 1);
			for (int i = 0; i < parameters[0].value/2; ++i)
			{
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				pushNewSubcane(&newSubcanes, p, VERTICAL_PICKUP_CANE_ORIENTATION, 1.0, width-0.0001, SQUARE_SHAPE);
				p.y = 0.0;
				pushNewSubcane(&newSubcanes, p, VERTICAL_PICKUP_CANE_ORIENTATION, 1.0, width-0.0001, SQUARE_SHAPE);
			}
			for (int i = 0; i < parameters[0].value; ++i)
			{
				p.x = 0.0 - width / 2 * (parameters[0].value % 2);
				p.y = -1.0 + width / 2 + width * i;                                
				pushNewSubcane(&newSubcanes, p, HORIZONTAL_PICKUP_CANE_ORIENTATION, 
					1.0 + width / 2 * (parameters[0].value % 2), width-0.0001, SQUARE_SHAPE);
			}
			break;
		case PickupTemplate::VERTICAL:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameters[0].value, 1);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				pushNewSubcane(&newSubcanes, p, VERTICAL_PICKUP_CANE_ORIENTATION, 2.0, width-0.0001, SQUARE_SHAPE);
			}
			break;
		case PickupTemplate::HORIZONTAL:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameters[0].value, 1);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0;
				p.y = -1.0 + width / 2 + width * i;
				pushNewSubcane(&newSubcanes, p, HORIZONTAL_PICKUP_CANE_ORIENTATION, 2.0, width-0.0001, SQUARE_SHAPE);
			}
			break;
		case PickupTemplate::VERTICAL_WITH_LIP_WRAP:
			p.x = p.y = p.z = 0.0;
			width = 2.0 / MAX(parameters[0].value, 1);
			p.x = -1.0;
			p.y = 1.0 - width/2;
			pushNewSubcane(&newSubcanes, p, HORIZONTAL_PICKUP_CANE_ORIENTATION, 2.0, width, SQUARE_SHAPE);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0 + width / 2 + width * i;
				p.y = -1.0;
				pushNewSubcane(&newSubcanes, p, VERTICAL_PICKUP_CANE_ORIENTATION, 2.0 - width, 
					width-0.0001, SQUARE_SHAPE);
			}
			break;
		case PickupTemplate::VERTICALS_AND_HORIZONTALS:
		{
			float verticals_width = 2.0 / MAX(parameters[0].value, 1);
			float horizontals_width = 1.0 / MAX(parameters[0].value/2, 1);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0 + verticals_width / 2 + verticals_width * i;
				p.y = 0.0;
				p.z = 0.0;
				pushNewSubcane(&newSubcanes, p, VERTICAL_PICKUP_CANE_ORIENTATION, 1.0, 
					verticals_width-0.0001, SQUARE_SHAPE);
				if (i % 2 == 1) {
					p.x = -1.0;
					p.y = -(horizontals_width / 2 + horizontals_width * (i-1) / 2);
					p.z = 0.0;
					pushNewSubcane(&newSubcanes, p, HORIZONTAL_PICKUP_CANE_ORIENTATION, 2.0, 
						horizontals_width-0.0001, SQUARE_SHAPE);
				}
			}
			break;
		}
		case PickupTemplate::VERTICAL_HORIZONTAL_VERTICAL:
		{
			float verticals_width = 2.0 / MAX(parameters[0].value, 1);
			float horizontals_width = 0.5 / MAX(parameters[0].value/3, 1);
			for (int i = 0; i < parameters[0].value; ++i) {
				p.x = -1.0 + verticals_width / 2 + verticals_width * i;
				p.y = -1.0;
				p.z = 0.0;
				pushNewSubcane(&newSubcanes, p, VERTICAL_PICKUP_CANE_ORIENTATION, 0.75,
					verticals_width-0.0001, SQUARE_SHAPE);
				p.x = -1.0 + verticals_width / 2 + verticals_width * i;
				p.y = 0.25;
				p.z = 0.0;
				pushNewSubcane(&newSubcanes, p, VERTICAL_PICKUP_CANE_ORIENTATION, 0.75,
					verticals_width-0.0001, SQUARE_SHAPE);
				if (i % 3 == 2) {
					p.x = -1.0;
					p.y = 0.25 - horizontals_width / 2 - horizontals_width * (i-2) / 3;
					p.z = 0.0;
					pushNewSubcane(&newSubcanes, p, HORIZONTAL_PICKUP_CANE_ORIENTATION, 2.0, 
						horizontals_width-0.0001, SQUARE_SHAPE);
				}
			}
			break;
		}
	}

	while (this->subcanes_.size() > 0)
	{
		SubpickupTemplate lastSubcane = this->subcanes_.back();
		this->subcanes_.pop_back();
		this->removeSubcaneDependency(lastSubcane.cane);
	}
	while (newSubcanes.size() > 0)
	{
		SubpickupTemplate lastSubcane = newSubcanes.back();
		newSubcanes.pop_back();
		this->subcanes_.push_back(lastSubcane);
		this->addSubcaneDependency(lastSubcane.cane);
	}

	emit modified();
}

void Piece :: setPickupTemplateType(enum PickupTemplate::Type _type, bool force) 
{
	if (!force && this->pickupType_ == _type)
		return;

	vector<TemplateParameter> &parameters = this->pickupParameters_;
	
	this->pickupType_ = _type;
	parameters.clear();
	switch (this->pickupType_) 
	{
		case PickupTemplate::VERTICALS_AND_HORIZONTALS:
			parameters.push_back(TemplateParameter(10, string("Column count:"), 6, 30));
			break;
		case PickupTemplate::VERTICAL:
			parameters.push_back(TemplateParameter(10, string("Column count:"), 6, 30));
			break;
		case PickupTemplate::HORIZONTAL:
			parameters.push_back(TemplateParameter(10, string("Row count:"), 6, 30));
			break;
		case PickupTemplate::MURRINE_COLUMN:
			parameters.push_back(TemplateParameter(10, string("Column count:"), 6, 30));
			break;
		case PickupTemplate::MURRINE_ROW:
			parameters.push_back(TemplateParameter(10, string("Column count:"), 6, 30));
			break;
		case PickupTemplate::MURRINE:
			parameters.push_back(TemplateParameter(6, string("Row/Column count:"), 6, 14));
			parameters.push_back(TemplateParameter(10, string("Thickness:"), 1, 40));
			break;
		case PickupTemplate::RETICELLO_VERTICAL_HORIZONTAL:
			parameters.push_back(TemplateParameter(10, string("Column count:"), 6, 30));
			break;
		case PickupTemplate::VERTICAL_HORIZONTAL_VERTICAL:
			parameters.push_back(TemplateParameter(10, string("Column count:"), 6, 20));
			break;
		case PickupTemplate::VERTICAL_WITH_LIP_WRAP:
			parameters.push_back(TemplateParameter(10, string("Column count:"), 6, 30));
			break;
	}

	updateSubcanes();
}

enum PickupTemplate::Type Piece :: pickupTemplateType() const 
{
	return this->pickupType_;
}

unsigned int Piece :: pickupParameterCount() const
{
	return this->pickupParameters_.size();
}

void Piece :: pickupParameter(unsigned int _index, TemplateParameter* dest) const
{
	assert(_index < this->pickupParameters_.size());
	*dest = this->pickupParameters_[_index];
}

void Piece :: setPickupParameter(unsigned int _index, int _value)
{
	assert(_index < this->pickupParameters_.size());
	this->pickupParameters_[_index].value = _value;
	updateSubcanes();
}

Piece *deep_copy(const Piece *_piece) 
{
	assert(_piece);
	Piece *piece = _piece->copy();
	for (unsigned int i = 0; i < _piece->subpickupCount(); ++i)
	{
		SubpickupTemplate t = _piece->subpickupTemplate(i);
		t.cane = deep_copy(t.cane);
		piece->setSubpickupTemplate(t, i);
	}
	return piece;

}

void deep_delete(Piece *piece) 
{
	assert(piece);
	vector<SubpickupTemplate> subcanes;
	for (unsigned int i = 0; i < piece->subpickupCount(); ++i)
		subcanes.push_back(piece->subpickupTemplate(i));
	for (unsigned int i = 0; i < subcanes.size(); ++i)
	{
		delete subcanes[i].cane;
		subcanes[i].cane = NULL;
	}	
	delete piece;
}

