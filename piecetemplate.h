
#ifndef PIECETEMPLATE_H
#define PIECETEMPLATE_H

#include <vector>
#include "primitives.h"
#include "constants.h"

#define TUMBLER_TEMPLATE 1
#define BOWL_TEMPLATE 2


using std::vector;

class PieceTemplate
{
	public:
		PieceTemplate(int t);
		int type;
		vector<char*> parameterNames;
		vector<int> parameterValues; 	
};

#endif

