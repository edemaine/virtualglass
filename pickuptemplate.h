
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
	enum Type
	{
		verticals,
		reticelloVerticalHorizontal,
		murrineColumn,
		verticalsAndHorizontals,
		verticalHorizontalVertical,
		verticalWithLipWrap,
		murrineRow,
		murrine
	};

	enum Type firstPickupTemplate();
	enum Type lastPickupTemplate();
}

#endif

