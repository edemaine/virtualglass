
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
		return TUMBLER;
	}

	enum Type lastSeedTemplate()
	{
		return WAVY_PLATE;
	}

	enum Type firstTemplate()
	{
		return TUMBLER;
	}

	enum Type lastTemplate()
	{
		return WAVY_PLATE;
	}

	const char* enumToString(enum Type t)
	{
		return names[t];
	}
}


