
#ifndef PICKUPTEMPLATE_H
#define PICKUPTEMPLATE_H

#include <vector>
#include "primitives.h"
#include "subpickuptemplate.h"
#include "constants.h"

// Pickup template types
#define TEN_VERTICALS_TEMPLATE 1
#define FIFTEEN_VERTICALS_TEMPLATE 2
#define TWENTY_VERTICALS_TEMPLATE 3
#define FOUR_SQUARES_OF_TEN_VERTICALS_TEMPLATE 4

using std::vector;

class PickupTemplate
{
	public:
		PickupTemplate(int t);
		vector<SubpickupTemplate> subpulls;
		int type;
};

#endif

