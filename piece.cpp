
#include "piece.h"

Piece :: Piece(int pickupTemplate)
{
	setTemplate(new PieceTemplate(pickupTemplate));

	// initialize the piece's pickup pickup to be something boring and base
	this->pickup = new PickupPlan(THREE_VERTICALS_TEMPLATE);
}

void Piece :: setTemplate(PieceTemplate* pt)
{
	this->pieceTemplate = pt;
}

PieceTemplate* Piece :: getTemplate()
{
	return this->pieceTemplate;
}

void Piece :: setPickup(PickupPlan* plan)
{
	// This is a kind of memory leak, as old pickup might have been the one allocated
	// when the piece was created (and now we are forgetting where it is).
	this->pickup = plan;
}

PickupPlan* Piece :: getPickup()
{
	return pickup;
}






