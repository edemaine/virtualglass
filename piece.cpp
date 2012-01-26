
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


bool Piece :: hasDependencyOn(Color* color)
{
	bool pickupPlansDependOn = false;

	for (unsigned int i = 0; i < this->pickup->subs.size(); ++i)
	{
		if (this->pickup->subs[i].plan->hasDependencyOn(color))
		{
			pickupPlansDependOn = true;		
			break;
		}
	}

	return pickupPlansDependOn;
}

/*
copy() is intended to be a copy at the correct depth consistent with
a Piece as represented in the GUI: it is a shape and a pickup plan, 
but does not include the pull plans used.
*/
Piece* Piece :: copy()
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







