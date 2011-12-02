
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
#define HORIZONTAL_LINE_PULL_TEMPLATE 6
#define CIRCLE_PULL_TEMPLATE 7
#define SURROUND_CIRCLE_PULL_TEMPLATE 8
#define SQUARE_PULL_TEMPLATE 9
#define BUNDLE_NINETEEN_TEMPLATE 10
#define X_NINE_TEMPLATE 11

#define FIRST_PULL_TEMPLATE CASED_CIRCLE_PULL_TEMPLATE
#define LAST_PULL_TEMPLATE SQUARE_PULL_TEMPLATE 

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
		int getShape();
		void setShape(int s);
		float getCasingThickness();
		void setCasingThickness(float t);
		bool isBase();

	private:
		// Methods
		void initializeSubtemps();
		void updateSubtemps();

		// Variables
		bool base;
		float casingThickness;
		int shape;	
		vector<int> parameterValues;
		vector<char*> parameterNames;
};

#endif

