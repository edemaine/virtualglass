

#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include "geometry.h"
#include "pullplan.h"
#include "pulltemplate.h"
#include "constants.h"

class Model
{
	public:
		Model();
		PullTemplate* getPullTemplate(int pt);
		Geometry* getGeometry(PullPlan* p);

	private:
		// Methods
		void initializePullTemplates();

		// Variables
		PullTemplate lineThreeCirclesPullTemplate;
		PullTemplate lineFiveCirclesPullTemplate;
		PullTemplate squareFourCirclesPullTemplate;
		PullTemplate xNineCirclesPullTemplate;
		Geometry* geometry;

		PullPlan* circleCasingPlan;
		PullPlan* squareCasingPlan;
};

#endif

