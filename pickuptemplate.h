
#ifndef PICKUPTEMPLATE_H
#define PICKUPTEMPLATE_H

// Pickup template types
namespace PickupTemplate
{
	// When adding a new template, be sure to update the names[] array 
	// and the first/last template functions in pickuptemplate.cpp
	enum Type
	{
		VERTICAL=1,
		RETICELLO_VERTICAL_HORIZONTAL,
		MURRINE_COLUMN,
		VERTICALS_AND_HORIZONTALS,
		VERTICAL_HORIZONTAL_VERTICAL,
		VERTICAL_WITH_LIP_WRAP,
		MURRINE_ROW,
		MURRINE
	};

	enum Type firstSeedTemplate();
	enum Type lastSeedTemplate();
}

#endif

