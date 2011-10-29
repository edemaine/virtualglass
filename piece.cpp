
#include "piece.h"

Piece :: Piece(int pickupTemplate)
{
	setTemplate(new PieceTemplate(pickupTemplate));

	// initialize the piece's pickup pickup to be something boring and base
	this->pickup = new PickupPlan(TWENTY_VERTICALS_TEMPLATE);
}

void Piece :: setTemplate(PieceTemplate* pt)
{
	this->pieceTemplate = pt;
}

PieceTemplate* Piece :: getTemplate()
{
	return this->pieceTemplate;
}







