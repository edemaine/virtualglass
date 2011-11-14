

#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include "geometry.h"
#include "pullplan.h"
#include "pickupplan.h"
#include "piece.h"

class Model
{
	public:
		Model();
		Geometry* getGeometry(PullPlan* p);
		Geometry* getGeometry(PickupPlan* p);
		Geometry* getGeometry(Piece* p);

	private:
		Geometry* pullPlanGeometry;
		Geometry* pickupPlanGeometry;
		Geometry* pieceGeometry;
		Mesher* mesher;
};

#endif

