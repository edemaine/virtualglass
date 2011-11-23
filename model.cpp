
#include "model.h"

Model :: Model()
{
	pullPlanGeometry = new Geometry();
	pickupPlanGeometry = new Geometry();
	pieceGeometry = new Geometry();
	mesher = new Mesher();
}

Geometry* Model :: getGeometry(PullPlan* plan)
{
	pullPlanGeometry->clear();
	mesher->generateMesh(plan, pullPlanGeometry);
	return pullPlanGeometry; 
}

Geometry* Model :: getGeometry(PickupPlan* plan)
{
	pickupPlanGeometry->clear();
	mesher->generateMesh(plan, pickupPlanGeometry);
	return pickupPlanGeometry;
}

Geometry* Model :: getGeometry(Piece* piece)
{
	pieceGeometry->clear();
	mesher->generateMesh(piece, pieceGeometry);
	return pieceGeometry;
}


