
#ifndef PIECE_H
#define PIECE_H

#include <vector>
#include <stack>

#include "templateparameter.h"
#include "piecetemplate.h"
#include "pickupplan.h"
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

		bool hasDependencyOn(GlassColor* c);
		bool hasDependencyOn(PullPlan* p);

		void setTwist(float t);
		float twist();
		float* twistPtr();

		void setSpline(Spline s);
		Spline spline();

		PickupPlan* pickup;

		void undo();
		void redo();
		bool canUndo();
		bool canRedo();
		void saveState();		
	
	private:

		struct State
		{
			float twist;
			enum PieceTemplate::Type type;
			Spline spline;
		};	

		stack<struct State> undoStackPiece;
		stack<struct PickupPlan::State> undoStackPickup;
		stack<struct State> redoStackPiece;
		stack<struct PickupPlan::State> redoStackPickup;
		struct State state;		
};

Piece *deep_copy(const Piece *);
void deep_delete(Piece *);

#endif

