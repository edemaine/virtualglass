
#include "piece.h"

Piece :: Piece(int pickupTemplate)
{
	setTemplate(new PieceTemplate(pickupTemplate));

	// initialize the piece's pickup pickup to be something boring and base
	this->pickup = new PickupPlan(VERTICALS_TEMPLATE);
}

void Piece :: setLibraryWidget(PieceLibraryWidget* plw)
{
	this->libraryWidget = plw;
}

PieceLibraryWidget* Piece :: getLibraryWidget()
{
	return this->libraryWidget;
}

void Piece :: setTemplate(PieceTemplate* pt)
{
	this->pieceTemplate = pt;
}

PieceTemplate* Piece :: getTemplate()
{
	return this->pieceTemplate;
}







