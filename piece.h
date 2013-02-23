
#ifndef PIECE_H
#define PIECE_H

#include <cstdlib>
#include <vector>
#include "templateparameter.h"
#include "piecetemplate.h"
#include "pickupplan.h"
#include "glasscolor.h"
#include "spline.h"

using std::vector;

class Piece
{
	public:
		Piece(enum PieceTemplate::Type t);

		void setTemplateType(enum PieceTemplate::Type t, bool force=false);
		enum PieceTemplate::Type getTemplateType();

		Piece* copy() const;
		PickupPlan* pickup;
		bool hasDependencyOn(GlassColor* c);
		bool hasDependencyOn(PullPlan* p);
		bool isBased();
	
		float twist;
		Spline spline;
	
	private:
		// Variables
		enum PieceTemplate::Type type;
		bool based;
};

Piece *deep_copy(const Piece *);
void deep_delete(Piece *);

#endif

