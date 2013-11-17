
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

		void setPickup(Pickup* p);
		Pickup* pickupPlan() const;

		void setSpline(Spline s);
		Spline spline();

		void undo();
		void redo();
		bool canUndo();
		bool canRedo();
		void saveState();		
	
	private:
		Pickup* pickup;

		struct State
		{
			float twist;
			enum PieceTemplate::Type type;
			Spline spline;
		};	

		void clearStateStacks();
		stack<struct State> undoStackPiece;
		stack<struct Pickup::State> undoStackPickup;
		stack<struct State> redoStackPiece;
		stack<struct Pickup::State> redoStackPickup;
		struct State state;		
};

Piece *deep_copy(const Piece *);
void deep_delete(Piece *);

#endif

