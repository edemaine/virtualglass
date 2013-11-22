
#ifndef PIECE_H
#define PIECE_H

#include <vector>
#include <stack>

#include "templateparameter.h"
#include "piecetemplate.h"
#include "pickup.h"
#include "glasscolor.h"
#include "spline.h"

using std::vector;
using std::stack;

class Piece
{
	public:
		Piece(enum PieceTemplate::Type t);

		void setTemplateType(enum PieceTemplate::Type t, bool force=false);
		enum PieceTemplate::Type templateType();

		Piece* copy() const;
		void set(Piece* p);

		bool hasDependencyOn(GlassColor* c);
		bool hasDependencyOn(Cane* p);

		void setTwist(float t);
		float twist();
		float* twistPtr();

		void setPickup(Pickup* p);
		Pickup* pickup() const;

		void setSpline(Spline s);
		Spline spline();

	private:
		Pickup* pickup_;

		float twist_;
		enum PieceTemplate::Type type_;
		Spline spline_;
};

Piece *deep_copy(const Piece *);
void deep_delete(Piece *);

#endif

