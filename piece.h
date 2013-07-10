
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
		enum PieceTemplate::Type templateType();
		Piece* copy() const;

		bool hasDependencyOn(GlassColor* c);
		bool hasDependencyOn(PullPlan* p);

		void setTwist(float t);
		float twist();
		float* twistPtr();

		PickupPlan* pickup;

		void setSpline(Spline s);
		Spline spline();
	
	private:
		struct State
		{
			float twist;
			enum PieceTemplate::Type type;
			Spline spline;
		};	

		struct State state;		
};

Piece *deep_copy(const Piece *);
void deep_delete(Piece *);

#endif

