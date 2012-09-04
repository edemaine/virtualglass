

#ifndef PIECE_H
#define PIECE_H

class PieceLibraryWidget;

#include <stdlib.h>
#include <vector>
#include "piecetemplate.h"
#include "pickupplan.h"
#include "glasscolor.h"

class Piece
{
	public:
		Piece(int pieceTemplate);

		void setTemplate(PieceTemplate* pt);
		PieceTemplate* getTemplate();

		Piece* copy() const;
		PickupPlan* pickup;
		bool hasDependencyOn(GlassColor* color);
		bool hasDependencyOn(PullPlan* pullPlan);
		
	//private:
		// Variables
		PieceTemplate* pieceTemplate;
};

Piece *deep_copy(const Piece *);
void deep_delete(Piece *);

#endif

