

#ifndef PIECE_H
#define PIECE_H

#include <stdlib.h>
#include <vector>
#include "piecetemplate.h"
#include "pickupplan.h"

class Piece
{
	public:
		Piece(int pieceTemplate);

		void setTemplate(PieceTemplate* pt);
		PieceTemplate* getTemplate();

		void setPickup(PickupPlan* pickup);
		PickupPlan* getPickup();
	
	private:
		// Variables
		PieceTemplate* pieceTemplate;
		PickupPlan* pickup;
};

#endif

