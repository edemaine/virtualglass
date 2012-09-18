
#ifndef PIECETEMPLATE_H
#define PIECETEMPLATE_H

namespace PieceTemplate
{
	enum Type
	{
		tumbler=1,
		bowl,
		vase,
		pot,
		wavyPlate
	};
	
	enum Type firstTemplate();
	enum Type lastTemplate();
}

#endif

