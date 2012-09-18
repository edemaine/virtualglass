
#ifndef PICKUPTEMPLATE_H
#define PICKUPTEMPLATE_H

// Pickup cane orientations
namespace PickupCane
{
	enum Orientation
	{
		horizontal,
		vertical,
		murrine
	};
}

// Pickup template types
namespace PickupTemplate
{
	// When adding a new template, be sure to update the names[] array 
	// and the first/last template functions in pickuptemplate.cpp
	enum Type
	{
		verticals=1,
		reticelloVerticalHorizontal,
		murrineColumn,
		verticalsAndHorizontals,
		verticalHorizontalVertical,
		verticalWithLipWrap,
		murrineRow,
		murrine
	};

	enum Type firstTemplate();
	enum Type lastTemplate();
	enum Type firstSeedTemplate();
	enum Type lastSeedTemplate();

        const char* enumToString(enum Type t);
}

#endif

