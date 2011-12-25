
#ifndef PULLTEMPLATE_H
#define PULLTEMPLATE_H

#include <vector>
#include "primitives.h"
#include "subpulltemplate.h"
#include "constants.h"

// Pull template types
#define CIRCLE_BASE_PULL_TEMPLATE 1
#define SQUARE_BASE_PULL_TEMPLATE 2
#define AMORPHOUS_BASE_PULL_TEMPLATE 3
#define CASED_CIRCLE_PULL_TEMPLATE 4
#define CASED_SQUARE_PULL_TEMPLATE 5
#define HORIZONTAL_LINE_CIRCLE_PULL_TEMPLATE 6
#define HORIZONTAL_LINE_SQUARE_PULL_TEMPLATE 7
#define SURROUNDING_CIRCLE_PULL_TEMPLATE 8
#define CROSS_PULL_TEMPLATE 9
#define SQUARE_OF_CIRCLES_PULL_TEMPLATE 10
#define SQUARE_OF_SQUARES_PULL_TEMPLATE 11
#define TRIPOD_PULL_TEMPLATE 12
#define SURROUNDING_SQUARE_PULL_TEMPLATE 13

#define FIRST_PULL_TEMPLATE CASED_CIRCLE_PULL_TEMPLATE
#define LAST_PULL_TEMPLATE TRIPOD_PULL_TEMPLATE

using std::vector;

class PullTemplate
{
	public:
		PullTemplate(int type);
		vector<SubpullTemplate> subtemps;
		int type;
                void setParameter(int param, int newValue);
                int getParameter(int param);
                char* getParameterName(int param);
		unsigned int getParameterCount();
		int getShape();
		void setShape(int s);
		float getCasingThickness();
		void setCasingThickness(float t);
		bool isBase();
		PullTemplate* copy();

	private:
		// Methods
		void initializeTemplate();
		void updateSubtemps();

		// Variables
		bool base;
		float casingThickness;
		int shape;	
		vector<int> parameterValues;
		vector<char*> parameterNames;
};

#endif

