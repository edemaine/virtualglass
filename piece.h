

#ifndef PIECE_H
#define PIECE_H

#include <stdlib.h>
#include <vector>
#include "templateparameter.h"
#include "piecetemplate.h"
#include "pickupplan.h"
#include "glasscolor.h"

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
		float getOuterZ();
		void setOuterZ(float);
		float getInnerZ();
		void setInnerZ(float);

		unsigned int getParameterCount();
		void getParameter(unsigned int index, TemplateParameter* dest);
		void setParameter(unsigned int index, int value);
		void vecLayerVerticesPushBack(unsigned int);
		unsigned int vecLayerVerticesGetValue(int);
		unsigned long vecLayerVerticesGetSize();
		void vecLayerVerticesSetValue(int, unsigned long);
		void vecLayerTwistSetValue(int, unsigned long);
		int vecLayerTwistGetValue(unsigned long);
		unsigned long vecLayerTwistGetSize();
		void vecLayerTwistPushBack(int);
	
	private:
		// Variables
		enum PieceTemplate::Type type;
		vector<TemplateParameter> parameters;
		float innerZ;
		float outerZ;
		vector<unsigned int>* vecLayerVertices;
		vector<int>* vecLayerTwist;
};

Piece *deep_copy(const Piece *);
void deep_delete(Piece *);

#endif

