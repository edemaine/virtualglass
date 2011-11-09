
#include "model.h"

Model :: Model()
{
	geometry = new Geometry();
	mesher = new Mesher();
}

Geometry* Model :: getGeometry(PullPlan* plan)
{
	geometry->clear();
	vector<PullPlan*> ancestors;
	vector<int> ancestorIndices;

	mesher->generateMesh(plan, geometry, ancestors, ancestorIndices, true);

	return geometry; 
}

Geometry* Model :: getGeometry(PickupPlan* plan)
{
	geometry->clear();
	vector<PullPlan*> ancestors;
	vector<int> ancestorIndices;

	mesher->generateMesh(plan, geometry, ancestors, ancestorIndices);

	return geometry;
}

Geometry* Model :: getGeometry(Piece* piece)
{
	geometry->clear();
	vector<PullPlan*> ancestors;
	vector<int> ancestorIndices;

	mesher->generateMesh(piece, geometry, ancestors, ancestorIndices);

	return geometry;
}


