
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
                "Wavy Plate",
		"Pickup"
        };

	enum Type firstSeedTemplate()
	{
		return TUMBLER;
	}

	enum Type lastSeedTemplate()
	{
		return PICKUP;
	}

	enum Type firstTemplate()
	{
		return TUMBLER;
	}

	enum Type lastTemplate()
	{
		return PICKUP;
	}

	const char* enumToString(enum Type t)
	{
		return names[t];
	}
}


