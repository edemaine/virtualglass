

#ifndef PIECE_H
#define PIECE_H

class PieceLibraryWidget;

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

		Piece* copy();
		PickupPlan* pickup;
		bool hasDependencyOn(Color* color);
		bool hasDependencyOn(PullPlan* pullPlan);
		
	private:
		// Variables
		PieceTemplate* pieceTemplate;
};

#endif

