

#ifndef MODEL_H
#define MODEL_H

#include "pulltemplate.h"
#include "constants.h"

class Model
{
	public:
		Model();
		PullTemplate* getPullTemplate(int pt);

	private:
		// Methods
		void initializePullTemplates();

		// Variables
		PullTemplate lineThreePullTemplate;
		PullTemplate lineFivePullTemplate;
};

#endif

