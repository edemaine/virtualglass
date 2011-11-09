
#ifndef PICKUPTEMPLATE_H
#define PICKUPTEMPLATE_H

#include <vector>
#include "primitives.h"
#include "subpickuptemplate.h"
#include "constants.h"

// Pickup cane orientations
#define HORIZONTAL_ORIENTATION 1
#define VERTICAL_ORIENTATION 2
#define MURRINE_ORIENTATION 3

// Pickup template types
#define TEN_VERTICALS_TEMPLATE 1
#define FIFTEEN_VERTICALS_TEMPLATE 2
#define TWENTY_VERTICALS_TEMPLATE 3
#define FOUR_SQUARES_OF_TEN_VERTICALS_TEMPLATE 4
#define SIXTEEN_MURRINE_TEMPLATE 5
#define FOUR_COLUMNS_MURRINE_TEMPLATE 6

using std::vector;

class PickupTemplate
{
	public:
		PickupTemplate(int t);
		vector<SubpickupTemplate> subpulls;
		int type;
};

#endif

