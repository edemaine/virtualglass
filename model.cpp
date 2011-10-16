
#include "model.h"

Model :: Model()
{
	// Initialize pull templates
	initializePullTemplates();
	geometry = new Geometry();
}

Geometry* Model :: getGeometry(PullPlan* plan)
{
	geometry->clear();
	vector<PullPlan*> ancestors;
	vector<int> ancestorIndices;

	generateMesh(plan, geometry, ancestors, ancestorIndices);

	return geometry; 
}

void Model :: initializePullTemplates()
{
	Point p;

	p.x = p.y = p.z = 0.0;

	for (int i = 0; i < 3; ++i)
	{
		p.x = -0.6666666 + 0.6666 * i;
		lineThreePullTemplate.addSubcane(p, 0.6666);
	}
	for (int i = 0; i < 5; ++i)
	{
		p.x = -0.8 + 0.4 * i;
		lineFivePullTemplate.addSubcane(p, 0.4);
	}
}

PullTemplate* Model :: getPullTemplate(int pt)
{
	switch (pt)
	{
		case LINE_THREE:
			return &lineThreePullTemplate;
		case LINE_FIVE:
			return &lineFivePullTemplate;
		default:
			return NULL;
	}
}


