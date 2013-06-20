
#include "piece.h"

Piece :: Piece(enum PieceTemplate::Type _type)
{
	twist = 0.0;
	setTemplateType(_type, true);
	// initialize the piece's pickup to be something boring and base
	this->pickup = new PickupPlan(PickupTemplate::VERTICAL);
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
	c->spline = this->spline;
	c->pickup = this->pickup->copy();
	
	return c;
}

void Piece :: setTemplateType(enum PieceTemplate::Type _type, bool force)
{
	if (!force && this->type == _type)
		return;

	this->type = _type;

	switch (this->type)
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

}

enum PieceTemplate::Type Piece :: templateType()
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
