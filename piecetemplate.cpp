
#include "piecetemplate.h"

namespace PieceTemplate
{
        const char* names[] =
        {
                "Undefined",
                "Tumbler",
                "Bowl",
                "Vase",
                "Pot",
                "Wavy Plate"
        };

	enum Type firstSeedTemplate()
	{
		return tumbler;
	}

	enum Type lastSeedTemplate()
	{
		return wavyPlate;
	}

	enum Type firstTemplate()
	{
		return tumbler;
	}

	enum Type lastTemplate()
	{
		return wavyPlate;
	}

	const char* enumToString(enum Type t)
	{
		return names[t];
	}
}


