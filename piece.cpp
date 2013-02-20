
#include "piece.h"

Piece :: Piece(enum PieceTemplate::Type _type)
{
	twist = 0.0;
	based = true;
	setTemplateType(_type, true);
	// initialize the piece's pickup to be something boring and base
	this->pickup = new PickupPlan(PickupTemplate::VERTICAL);
}

bool Piece :: isBased()
{
	return based;
}

bool Piece :: hasDependencyOn(PullPlan* plan)
{
	bool pickupPlansDependOn = false;

	for (unsigned int i = 0; i < this->pickup->subs.size(); ++i)
	{
		if (this->pickup->subs[i].plan->hasDependencyOn(plan))
		{
			pickupPlansDependOn = true;
			break;
		}
	}

	return pickupPlansDependOn;
}

bool Piece :: hasDependencyOn(GlassColor* glassColor)
{
	bool pickupPlansDependOn = false;

	for (unsigned int i = 0; i < pickup->subs.size(); ++i)
	{
		if (pickup->subs[i].plan->hasDependencyOn(glassColor))
		{
			pickupPlansDependOn = true;		
			break;
		}
	}
	
	if (pickup->overlayGlassColor == glassColor || pickup->underlayGlassColor == glassColor)
		pickupPlansDependOn = true;

	return pickupPlansDependOn;
}

/*
copy() is intended to be a copy at the correct depth consistent with
a Piece as represented in the GUI: it is a shape and a pickup plan, 
but does not include the pull plans used.
*/
Piece* Piece :: copy() const
{
	Piece* c = new Piece(type);

	c->twist = this->twist;
	c->based = this->based;
	c->spline = this->spline;
	c->pickup = this->pickup->copy();
	
	return c;
}

void Piece :: setTemplateType(enum PieceTemplate::Type _type, bool force)
{
	if (!force && this->type == _type)
		return;

	this->type = _type;

	spline.clear();
	spline.push_back(1.0);
	spline.push_back(1.0);
	spline.push_back(1.0);
	spline.push_back(1.0);
	switch (type)
	{
		case PieceTemplate::TUMBLER:
			based = true;
			spline[0] = 3.0;
			spline[1] = 3.0;
			spline[2] = 3.0;
			spline[3] = 3.0;
			break;
		case PieceTemplate::VASE:
			based = true;
			spline[0] = 1.0;
			spline[1] = 1.0;
			spline[2] = 1.0;
			spline[3] = 5.0;
			break;
		case PieceTemplate::BOWL:
			based = true;
			spline[0] = 1.0;
			spline[1] = 1.0;
			spline[2] = 1.0;
			spline[3] = 5.0;
			break;
		case PieceTemplate::POT:
			based = false;
			spline[0] = 1.0;
			spline[1] = 1.0;
			spline[2] = 1.0;
			spline[3] = 5.0;
			break;
		case PieceTemplate::PLATE:
			based = true;
			spline[0] = 8.0;
			spline[1] = 8.0;
			spline[2] = 8.0;
			spline[3] = 8.0;
			break;
		case PieceTemplate::PICKUP:
			based = false;
			break;
	}

}

enum PieceTemplate::Type Piece :: getTemplateType()
{
	return type;
}

Piece *deep_copy(const Piece *_piece) {
	assert(_piece);
	Piece *piece = _piece->copy();
	//Replace pickup with a deep copy:
	delete piece->pickup;
	piece->pickup = deep_copy(_piece->pickup);
	return piece;
}

void deep_delete(Piece *piece) {
	assert(piece);
	deep_delete(piece->pickup);
	piece->pickup = NULL;
	delete piece;
}
