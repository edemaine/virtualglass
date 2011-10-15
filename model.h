

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
		PullTemplate lineThreePullTemplate;
		PullTemplate lineFivePullTemplate;
		Geometry* geometry;
};

#endif

