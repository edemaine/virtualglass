
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
	vector<PullPlan*> ancestors;
	vector<int> ancestorIndices;

	mesher->updateTotalCaneLength(plan);
	mesher->generateMesh(plan, pullPlanGeometry, &ancestors, &ancestorIndices, 0.0, 1.0, true);

	return pullPlanGeometry; 
}

Geometry* Model :: getGeometry(PickupPlan* plan)
{
	pickupPlanGeometry->clear();
	vector<PullPlan*> ancestors;
	vector<int> ancestorIndices;

	mesher->updateTotalCaneLength(plan);
	mesher->generateMesh(plan, pickupPlanGeometry, &ancestors, &ancestorIndices);

	return pickupPlanGeometry;
}

Geometry* Model :: getGeometry(Piece* piece)
{
	pieceGeometry->clear();
	vector<PullPlan*> ancestors;
	vector<int> ancestorIndices;

	mesher->updateTotalCaneLength(piece);
	mesher->generateMesh(piece, pieceGeometry, &ancestors, &ancestorIndices);

	return pieceGeometry;
}


