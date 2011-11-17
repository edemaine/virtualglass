
#ifndef PULLTEMPLATE_H
#define PULLTEMPLATE_H

#include <vector>
#include "primitives.h"
#include "subpulltemplate.h"
#include "constants.h"

// Pull template types
#define CIRCLE_BASE_TEMPLATE 1
#define SQUARE_BASE_TEMPLATE 2
#define AMORPHOUS_BASE_TEMPLATE 3
#define CASED_CIRCLE_TEMPLATE 4
#define LINE_THREE_TEMPLATE 5
#define LINE_FIVE_TEMPLATE 6
#define CIRCLE_FOUR_TEMPLATE 7
#define CIRCLE_SIX_TEMPLATE 8
#define CIRCLE_TWELVE_TEMPLATE 9
#define SQUARE_FOUR_TEMPLATE 10
#define SQUARE_SIXTEEN_TEMPLATE 11
#define BUNDLE_NINETEEN_TEMPLATE 12
#define X_NINE_TEMPLATE 13

#define FIRST_TEMPLATE CASED_CIRCLE_TEMPLATE
#define LAST_TEMPLATE X_NINE_TEMPLATE

using std::vector;

class PullTemplate
{
	public:
		PullTemplate(int type, float casingThickness);
		vector<SubpullTemplate> subpulls;
		int shape;	
		int type;
		void setCasingThickness(float t);
		float getCasingThickness();

	private:
		// Methods
		void initializeSubpulls();
		void updateSubpulls();

		// Variables
		float casingThickness;
};

#endif

