
#ifndef PIECETEMPLATE_H
#define PIECETEMPLATE_H

namespace PieceTemplate
{
	// When adding a new template, be sure to update the 
	// first/last template functions in piecetemplate.cpp
	enum Type
	{
		TUMBLER=1,
		BOWL,
		VASE,
		POT,
		PLATE,
		FISHTRAP,
		CUSTOM
	};
	
	enum Type firstSeedTemplate();
	enum Type lastSeedTemplate();
}

#endif

