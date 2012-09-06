
#include "piece.h"

Piece :: Piece(int pickupTemplate)
{
	setTemplate(new PieceTemplate(pickupTemplate));
	// initialize the piece's pickup to be something boring and base
	this->pickup = new PickupPlan(VERTICALS_PICKUP_TEMPLATE);
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
	Piece* c = new Piece(this->pieceTemplate->type);

	c->pieceTemplate = this->pieceTemplate->copy();
	c->pickup = this->pickup->copy();
	
	return c;
}

void Piece :: setTemplate(PieceTemplate* pt)
{
	this->pieceTemplate = pt;
}

PieceTemplate* Piece :: getTemplate()
{
	return this->pieceTemplate;
}



Piece *deep_copy(const Piece *_piece) {
	assert(_piece);
	Piece *piece = _piece->copy();
	//Replace piece with a deep copy:
	delete piece->pickup;
	piece->pickup = deep_copy(_piece->pickup);
	return piece;
}

void deep_delete(Piece *piece) {
	assert(piece);
	deep_delete(piece->pickup);
	piece->pickup = NULL;
	delete piece->pieceTemplate;
	piece->pieceTemplate = NULL;
	delete piece;
}
