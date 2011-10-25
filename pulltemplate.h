
#ifndef PULLTEMPLATE_H
#define PULLTEMPLATE_H

#include <vector>
#include "primitives.h"
#include "subpulltemplate.h"
#include "constants.h"

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

