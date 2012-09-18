
#ifndef PIECETEMPLATE_H
#define PIECETEMPLATE_H

namespace PieceTemplate
{
	// When adding a new template, be sure to update the 
	// names[] array and the first/last template functions in piecetemplate.cpp
	enum Type
	{
		tumbler=1,
		bowl,
		vase,
		pot,
		wavyPlate
	};
	
	enum Type firstSeedTemplate();
	enum Type lastSeedTemplate();
	enum Type firstTemplate();
	enum Type lastTemplate();

	const char* enumToString(enum Type t);
}

#endif

