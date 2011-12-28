
#ifndef PIECETEMPLATE_H
#define PIECETEMPLATE_H

#include <vector>
#include "primitives.h"
#include "constants.h"

#define TUMBLER_PIECE_TEMPLATE 1
#define BOWL_PIECE_TEMPLATE 2
#define VASE_PIECE_TEMPLATE 3
#define POT_PIECE_TEMPLATE 4
#define WAVY_PLATE_PIECE_TEMPLATE 5

#define FIRST_PIECE_TEMPLATE TUMBLER_PIECE_TEMPLATE
#define LAST_PIECE_TEMPLATE WAVY_PLATE_PIECE_TEMPLATE

using std::vector;

class PieceTemplate
{
	public:
		PieceTemplate(int t);
		int type;
		vector<char*> parameterNames;
		vector<int> parameterValues; 	
		PieceTemplate* copy();
};

#endif

