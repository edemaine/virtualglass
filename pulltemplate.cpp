
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
		"Custom",
	};

	enum Type firstSeedTemplate()
	{
		return BASE_CIRCLE;
	}

	enum Type lastSeedTemplate()
	{
		return SURROUNDING_SQUARE;
	}

	// returns true only if it's for sure
	bool templateHasNoSubplans(PullTemplate::Type t)
	{
		switch (t)
		{
			case BASE_CIRCLE:
			case BASE_SQUARE:
				return true;
			default:
				return false;
		}
	}

	// returns true only if it's for sure
	bool templateHasSubplans(PullTemplate::Type t)
	{
		switch (t)
		{
			case BASE_CIRCLE:
			case BASE_SQUARE:
			case CUSTOM:
				return false;
			default:
				return true;
		}
	}
}


