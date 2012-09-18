

#ifndef PIECE_H
#define PIECE_H

#include <stdlib.h>
#include <vector>
#include "piecetemplate.h"
#include "pickupplan.h"
#include "glasscolor.h"

using std::vector;

class Piece
{
	public:
		Piece(enum PieceTemplate::Type type);

		void setTemplateType(enum PieceTemplate::Type type, bool force=false);
		enum PieceTemplate::Type getTemplateType();

		Piece* copy() const;
		PickupPlan* pickup;
		bool hasDependencyOn(GlassColor* color);
		bool hasDependencyOn(PullPlan* pullPlan);

		unsigned getParameterCount();
		int getParameter(unsigned int index);
		char* getParameterName(unsigned int index);
		void setParameter(unsigned int index, int value);
	
	private:
		// Variables
		enum PieceTemplate::Type type;
		vector<int> parameterValues;
                vector<char*> parameterNames;
};

Piece *deep_copy(const Piece *);
void deep_delete(Piece *);

#endif

