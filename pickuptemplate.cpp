
#include "pickuptemplate.h"

namespace PickupTemplate
{
	const char* names[] =
	{
		"Undefined",
		"Vertical",
		"Reticello Vertical Horizontal",
		"Murrine Column",
		"Verticals and Horizontals",
		"Vertical Horizontal Vertical",
		"Vertical With Lip Wrap",
		"Murrine Row",
		"Murrine"
	};

	enum Type firstTemplate()
	{
		return VERTICAL;
	}

	enum Type lastTemplate()
	{
		return MURRINE;
	}

	enum Type firstSeedTemplate()
	{
		return VERTICAL;
	}

	enum Type lastSeedTemplate()
	{
		return MURRINE;
	}

	const char* enumToString(enum Type t)
	{
		return names[t];
	}
}

