
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
}

#endif

