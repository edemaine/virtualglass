
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
#define VERTICALS_PICKUP_TEMPLATE 1
#define RETICELLO_VERTICAL_HORIZONTAL_PICKUP_TEMPLATE 2
#define MURRINE_COLUMN_PICKUP_TEMPLATE 3
#define VERTICALS_AND_HORIZONTALS_PICKUP_TEMPLATE 4
#define VERTICAL_HORIZONTAL_VERTICAL_PICKUP_TEMPLATE 5
#define VERTICAL_WITH_LIP_WRAP_PICKUP_TEMPLATE 6

#define FIRST_PICKUP_TEMPLATE VERTICALS_PICKUP_TEMPLATE
#define LAST_PICKUP_TEMPLATE VERTICAL_WITH_LIP_WRAP_PICKUP_TEMPLATE

using std::vector;

class PickupTemplate
{
	public:
		PickupTemplate(int t);
		vector<SubpickupTemplate*> subtemps;
		int type;
		void setParameter(int param, int newValue);
		int getParameter(int param);
		char* getParameterName(int param);	
		PickupTemplate* copy();

	private:
		vector<int> parameterValues; 
		vector<char*> parameterNames;
		void computeSubtemps();
};

#endif

