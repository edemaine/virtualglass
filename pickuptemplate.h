
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
#define VERTICALS_TEMPLATE 1
#define MURRINE_SQUARE_TEMPLATE 2

using std::vector;

class PickupTemplate
{
	public:
		PickupTemplate(int t);
		vector<SubpickupTemplate*> subpulls;
		int type;
		void setParameter(int param, int newValue);
		int getParameter(int param);
		char* getParameterName(int param);	

	private:
		vector<int> parameterValues; 
		vector<char*> parameterNames;
		void computeSubpulls();
};

#endif

