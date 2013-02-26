
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

	if (this->type != PieceTemplate::CUSTOM)
		spline.reset();
	switch (this->type)
	{
		case PieceTemplate::TUMBLER:
			based = true;
			spline.set(0, 3.0);
			spline.set(1, 3.0);
			spline.set(2, 3.0);
			spline.set(3, 3.0);
			break;
		case PieceTemplate::VASE:
			based = true;
			spline.set(0, 1.3);
			spline.set(1, 4.0);
			spline.set(2, 0.0);
			spline.set(3, 2.0);
			break;
		case PieceTemplate::BOWL:
			based = true;
			spline.set(0, 2.8);
			spline.set(1, 6.9);
			spline.set(2, 7.0);
			spline.set(3, 7.2);
			break;
		case PieceTemplate::POT:
			based = true;
			spline.set(0, 2.0);
			spline.set(1, 3.5);
			spline.set(2, 3.5);
			spline.set(3, 2.0);
			break;
		case PieceTemplate::PLATE:
			based = true;
			spline.set(0, 9.0);
			spline.set(1, 9.1);
			spline.set(2, 9.2);
			spline.set(3, 9.3);
			break;
		case PieceTemplate::CUSTOM:
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
