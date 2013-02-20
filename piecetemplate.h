
#ifndef PIECETEMPLATE_H
#define PIECETEMPLATE_H

namespace PieceTemplate
{
	// When adding a new template, be sure to update the 
	// names[] array and the first/last template functions in piecetemplate.cpp
	enum Type
	{
		TUMBLER=1,
		BOWL,
		VASE,
		POT,
		PLATE,
		CUSTOM
	};
	
	enum Type firstSeedTemplate();
	enum Type lastSeedTemplate();
}

#endif

