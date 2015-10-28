
#ifndef PICKUPTEMPLATE_H
#define PICKUPTEMPLATE_H

// Pickup template types
namespace PickupTemplate
{
	// When adding a new template, also update the following:
	// 1. firstSeedTemplate(), lastSeedTemplate() in pickuptemplate.cpp
	// 2. pickupTemplateToString(), stringToPickupTemplate() in glassfileio.cpp
	// 3. setPickupTemplateType(), updateSubcanes() in piece.cpp
	// 4. pickuptemplate*.png in ./images/
	// 5. virtualglass.qrc file.
	enum Type
	{
		VERTICAL=1,
		HORIZONTAL,
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

