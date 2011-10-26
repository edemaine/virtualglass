

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
		
		PickupPlan* pickup;

	private:
		// Variables
		PieceTemplate* pieceTemplate;
};

#endif

