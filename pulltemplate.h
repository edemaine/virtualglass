
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
#define LINE_THREE_CIRCLES_TEMPLATE 5
#define LINE_FIVE_CIRCLES_TEMPLATE 6
#define LINE_THREE_SQUARES_TEMPLATE 7
#define LINE_FIVE_SQUARES_TEMPLATE 8
#define CIRCLE_FOUR_CIRCLES_TEMPLATE 9
#define CIRCLE_SIX_CIRCLES_TEMPLATE 10
#define CIRCLE_TWELVE_CIRCLES_TEMPLATE 11
#define SQUARE_FOUR_SQUARES_TEMPLATE 12
#define SQUARE_SIXTEEN_SQUARES_TEMPLATE 13
#define BUNDLE_NINETEEN_CIRCLES_TEMPLATE 14
#define X_NINE_CIRCLES_TEMPLATE 15

#define FIRST_TEMPLATE CASED_CIRCLE_TEMPLATE
#define LAST_TEMPLATE X_NINE_CIRCLES_TEMPLATE

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

