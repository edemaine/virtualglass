

#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include "geometry.h"
#include "pullplan.h"

class Model
{
	public:
		Model();
		Geometry* getGeometry(PullPlan* p);

	private:
		Geometry* geometry;
		PullPlan* circleCasing;
		PullPlan* squareCasing;
};

#endif

