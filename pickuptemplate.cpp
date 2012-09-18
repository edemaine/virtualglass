
#include "pickuptemplate.h"

namespace PickupTemplate
{
        const char* names[] =
        {
                "Undefined",
                "Verticals",
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
		return verticals;
	}

	enum Type lastTemplate()
	{
		return murrine;
	}

	enum Type firstSeedTemplate()
	{
		return verticals;
	}

	enum Type lastSeedTemplate()
	{
		return murrine;
	}

        const char* enumToString(enum Type t)
        {
                return names[t];
        }
}

