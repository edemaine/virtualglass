
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
		return BASE_CIRCLE;
	}

	enum Type lastSeedTemplate()
	{
		return SURROUNDING_SQUARE;
	}

	enum Type firstTemplate()
	{
		return BASE_CIRCLE;
	}

	enum Type lastTemplate()
	{
		return CUSTOM_SQUARE;
	}

	const char* enumToString(enum Type t)
	{
		return names[t];
	}
}


