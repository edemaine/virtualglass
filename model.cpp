
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

	// Line of three circles 
	for (int i = 0; i < 3; ++i)
	{
		p.x = -0.6666666 + 0.6666 * i;
		lineThreeCirclesPullTemplate.addSubpullTemplate(SubpullTemplate(CIRCLE_SHAPE, p, 0.65, 0));
	}

	// Line of five circles
	for (int i = 0; i < 5; ++i)
	{
		p.x = -0.8 + 0.4 * i;
		lineFiveCirclesPullTemplate.addSubpullTemplate(SubpullTemplate(CIRCLE_SHAPE, p, 0.39, 0));
	}

	// Square of four circles
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			p.x = -0.3 + 0.6 * i;
			p.y = -0.3 + 0.6 * j;
			squareFourCirclesPullTemplate.addSubpullTemplate(SubpullTemplate(CIRCLE_SHAPE, p, 0.59, 0));
		}
	}

	// X of nine circles
	for (int i = 0; i < 5; ++i)
	{
		p.x = -0.8 + 0.4 * i;
		p.y = 0.0;
		xNineCirclesPullTemplate.addSubpullTemplate(SubpullTemplate(CIRCLE_SHAPE, p, 0.39, 0));
	}
	for (int i = 0; i < 5; ++i)
	{
		if (i == 2)
			continue;
		p.x = 0.0;
		p.y = -0.8 + 0.4 * i;
		xNineCirclesPullTemplate.addSubpullTemplate(SubpullTemplate(CIRCLE_SHAPE, p, 0.39, 0));
	}
}

PullTemplate* Model :: getPullTemplate(int pt)
{
	switch (pt)
	{
		case LINE_THREE_CIRCLES:
			return &lineThreeCirclesPullTemplate;
		case LINE_FIVE_CIRCLES:
			return &lineFiveCirclesPullTemplate;
		case SQUARE_FOUR_CIRCLES:
			return &squareFourCirclesPullTemplate;
		case X_NINE_CIRCLES:
			return &xNineCirclesPullTemplate;
		default:
			return NULL;
	}
}


