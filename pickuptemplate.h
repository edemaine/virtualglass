
#ifndef PICKUPTEMPLATE_H
#define PICKUPTEMPLATE_H

#include <vector>
#include "primitives.h"
#include "subpickuptemplate.h"
#include "constants.h"

// Pickup template types
#define TWENTY_HORIZONTALS_TEMPLATE 1
#define TEN_AND_TEN_HORIZONTALS_TEMPLATE 2
#define TWENTY_VERTICALS_TEMPLATE 3
#define TEN_AND_TEN_VERTICALS_TEMPLATE 4
#define SIX_COLUMNS_OF_TWENTY_HORIZONTALS_TEMPLATE 5
#define FOUR_SQUARES_OF_TEN_VERTICALS_TEMPLATE 6

using std::vector;

class PickupTemplate
{
	public:
		PickupTemplate(int t);
		vector<SubpickupTemplate> subpulls;
		int type;
};

#endif

