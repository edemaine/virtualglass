
#ifndef PULLTEMPLATE_H
#define PULLTEMPLATE_H

#include <vector>
#include "primitives.h"
#include "subpulltemplate.h"
#include "constants.h"

// Pull template types
#define LINE_THREE_CIRCLES_TEMPLATE 1
#define LINE_FIVE_CIRCLES_TEMPLATE 2
#define SQUARE_FOUR_CIRCLES_TEMPLATE 3
#define X_NINE_CIRCLES_TEMPLATE 4
#define SQUARE_FOUR_SQUARES_TEMPLATE 5
#define ONE_COLORED_CASING_TEMPLATE 6
#define SQUARE_SIXTEEN_SQUARES_TEMPLATE 7
#define CIRCLE_BASE_TEMPLATE 8
#define SQUARE_BASE_TEMPLATE 9
#define AMORPHOUS_BASE_TEMPLATE 10

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

