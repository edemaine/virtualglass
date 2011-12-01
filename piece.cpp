
#include "piece.h"

Piece :: Piece(int pickupTemplate)
{
	setTemplate(new PieceTemplate(pickupTemplate));
	this->libraryWidget = NULL;
	// initialize the piece's pickup to be something boring and base
	this->pickup = new PickupPlan(VERTICALS_TEMPLATE);
}

/*
copy() is intended to be a copy at the correct depth consistent with
a Piece as represented in the GUI: it is a shape and a pickup plan, 
but does not include the pull plans used.
*/
Piece* Piece :: copy()
{
	Piece* c = new Piece(this->pieceTemplate->type);

	c->libraryWidget = this->libraryWidget; 
	c->pieceTemplate = this->pieceTemplate->copy();
	c->pickup = this->pickup->copy();
	
	return c;
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







