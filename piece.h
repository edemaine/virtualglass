

#ifndef PIECE_H
#define PIECE_H

class PieceLibraryWidget;

#include <stdlib.h>
#include <vector>
#include "piecelibrarywidget.h"
#include "piecetemplate.h"
#include "pickupplan.h"

class Piece
{
	public:
		Piece(int pieceTemplate);

		void setLibraryWidget(PieceLibraryWidget* plw);
		PieceLibraryWidget* getLibraryWidget();

		void setTemplate(PieceTemplate* pt);
		PieceTemplate* getTemplate();

		Piece* copy();
		PickupPlan* pickup;
		bool hasDependencyOn(Color* color);
		bool hasDependencyOn(PullPlan* pullPlan);
		
	private:
		// Variables
		PieceTemplate* pieceTemplate;
		PieceLibraryWidget* libraryWidget;
};

#endif

