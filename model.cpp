
#include "model.h"

Model :: Model()
{
	geometry = new Geometry();
	Color color;
	color.r = color.g = color.b = 1.0;
	color.a = 0.2;
	circleCasing = new PullPlan(CIRCLE_BASE_TEMPLATE, true, color); 
	squareCasing = new PullPlan(SQUARE_BASE_TEMPLATE, true, color); 
}

Geometry* Model :: getGeometry(PullPlan* plan)
{
	geometry->clear();
	vector<PullPlan*> ancestors;
	vector<int> ancestorIndices;

	switch (plan->getTemplate()->shape)
	{
		case CIRCLE_SHAPE:
			generateMesh(plan, geometry, ancestors, ancestorIndices, circleCasing);
			break;
		case SQUARE_SHAPE:
			generateMesh(plan, geometry, ancestors, ancestorIndices, squareCasing);
			break;
		default:
			generateMesh(plan, geometry, ancestors, ancestorIndices);
			break;
	}

	return geometry; 
}

Geometry* Model :: getGeometry(PickupPlan* plan)
{
	geometry->clear();
	vector<PullPlan*> ancestors;
	vector<int> ancestorIndices;

	generateMesh(plan, geometry, ancestors, ancestorIndices);

	return geometry;
}

Geometry* Model :: getGeometry(Piece* piece)
{
	geometry->clear();
	vector<PullPlan*> ancestors;
	vector<int> ancestorIndices;

	generateMesh(piece, geometry, ancestors, ancestorIndices);

	return geometry;
}


