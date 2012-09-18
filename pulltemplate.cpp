
#include "pulltemplate.h"

// Pull template types
namespace PullTemplate
{
        const char* names[] = 
        {
                "Undefined",
                "Base Circle",
                "Base Square",
                "Horizontal Line Circle",
                "Horizontal Line Square",
                "Tripod",
                "Cross",
                "Square of Circles",
                "Square of Squares",
                "Surrounding Circle",
                "Surrounding Square",
                "Custom Circle",
                "Custom Square"
        };

	enum Type firstSeedTemplate()
	{
		return baseCircle;
	}

	enum Type lastSeedTemplate()
	{
		return surroundingSquare;
	}

	enum Type firstTemplate()
	{
		return baseCircle;
	}

	enum Type lastTemplate()
	{
		return customSquare;
	}

	const char* enumToString(enum Type t)
	{
		return names[t];
	}
}


