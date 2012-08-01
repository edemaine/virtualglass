
#include "mesh.h"

Mesher :: Mesher()
{
}

void Mesher:: updateTotalCaneLength(Piece* piece)
{
	totalCaneLength = computeTotalCaneLength(piece);
}

void Mesher:: updateTotalCaneLength(PickupPlan* plan)
{
	totalCaneLength = computeTotalCaneLength(plan);
}

void Mesher:: updateTotalCaneLength(PullPlan* plan)
{
	totalCaneLength = computeTotalCaneLength(plan);
}

float Mesher :: computeTotalCaneLength(Piece* piece)
{
	return computeTotalCaneLength(piece->pickup);
}

float Mesher :: computeTotalCaneLength(PickupPlan* plan)
{
	float total = 0.0;
	for (unsigned int i = 0; i < plan->subs.size(); ++i)
	{
		total += computeTotalCaneLength(plan->subs[i].plan) * plan->subs[i].length / 2.0;
	}
	return total;
}

float Mesher :: computeTotalCaneLength(PullPlan* plan)
{
	if (plan->isBase())
		return 1.0;
	
	float total = 0.0;
	for (unsigned int i = 0; i < plan->subs.size(); ++i)
	{
		total += computeTotalCaneLength(plan->subs[i].plan);
	}
	return total;	
}

void Mesher :: applyMoveAndResizeTransform(Geometry* geometry, PullPlan* parentPlan, int subplan)
{
	for (uint32_t v = 0; v < geometry->vertices.size(); ++v)
	{
		applyMoveAndResizeTransform(&(geometry->vertices[v]), parentPlan, subplan);
	}
}

void Mesher :: applyResizeTransform(Vertex* v, float scale)
{
	// Adjust diameter
	v->position.x *= scale; 
	v->position.y *= scale; 
}

void Mesher :: applyMoveAndResizeTransform(Vertex* v, PullPlan* parentNode, int subplan)
{
	SubpullTemplate* subTemp = &(parentNode->subs[subplan]);
	Point locationInParent = subTemp->location;
	float diameter = subTemp->diameter;

	// Adjust diameter
	v->position.x *= diameter / 2.0; 
	v->position.y *= diameter / 2.0; 

	// Adjust to location in parent
	v->position.x += locationInParent.x;
	v->position.y += locationInParent.y;
}

void Mesher :: applyTwistTransform(Geometry* geometry, PullPlan* transformNode)
{
	for (uint32_t v = 0; v < geometry->vertices.size(); ++v)
	{
		applyTwistTransform(&(geometry->vertices[v]), transformNode);
	}
}

void Mesher :: applyTwistTransform(Vertex* v, PullPlan* transformNode)
{
	float twist = transformNode->getTwist();

	// Apply twist
	float preTheta = atan2(v->position.y, v->position.x);
	float r = length(v->position.xy);
	float transformTheta = twist / 10.0 * v->position.z;
	float postTheta = preTheta + transformTheta;
	v->position.x = r * cos(postTheta);
	v->position.y = r * sin(postTheta);
}

void Mesher :: applyPickupTransform(Vertex* v, SubpickupTemplate* spt)
{
	// Shrink width to correct width
	v->position.x *= spt->width * 2.5; 
	v->position.y *= spt->width * 2.5; 

	// Change orientation if needed
	float tmp;
	switch (spt->orientation)
	{
		case HORIZONTAL_ORIENTATION:
			tmp = v->position.x;
			v->position.x = v->position.z;
			v->position.z = -tmp;
			break;
		case VERTICAL_ORIENTATION:
			break;
		case MURRINE_ORIENTATION:
			tmp = v->position.y;
			v->position.y = v->position.z;
			v->position.z = -tmp;
			break;
	}

	// Offset by location
	v->position.x = v->position.x + spt->location.x * 5.0;
	v->position.z = v->position.z + spt->location.y * 5.0;
	v->position.y = v->position.y + spt->location.z * 5.0;
}

void Mesher :: applyBowlTransform(Vertex* v, vector<int>* parameterValues)
{
	int radius = (*parameterValues)[0];
	float size = 1.0 + 0.01 * (*parameterValues)[1];
	int twist = (*parameterValues)[2];

	float theta = PI * v->position.x / 5.0 + PI * v->position.z * twist / 500.0;
	float totalR = 4.0 + radius * 0.1;
	float totalPhi = 10.0 / totalR;
	float r = totalR*size - v->position.y/size;
	float phi = ((v->position.z - -5.0) / 10.0) * totalPhi - PI/2;

	v->position.x = r * cos(theta) * cos(phi);
	v->position.y = r * sin(theta) * cos(phi);
	v->position.z = r * sin(phi) + (totalR - totalR * sin(totalPhi - PI / 2))/2.0;
}

float Mesher :: splineVal(float r1, float r2, float r3, float r4, float t)
{
	return pow((1.0 - t), 3) * r1 + 3 * pow(1.0 - t, 2) * t * r2 + 3 * pow(1.0 - t, 2) * t * r3 + pow(t, 3) * r4;
}

float Mesher :: splineVal(float r1, float r2, float r3, float t)
{
	return pow((1.0 - t), 2) * r1 + 2 * (1.0 - t) * t * r2 + pow(t, 2) * r3;
}

void Mesher :: applyWavyPlateTransform(Vertex* v, vector<int>* parameterValues)
{
	// Do a rollup
	// send x value to theta value 
	// -5.0 goes to -PI, 5.0 goes to PI
	// everything gets a base radius of 5.0
	float theta = PI * v->position.x / 5.0;

	float totalR = 4.0 + 100 * 0.1;
	float totalPhi = 10.0 / totalR;

	float r = totalR - v->position.y;
	float phi = ((v->position.z - -5.0) / 10.0) * totalPhi - PI/2;

	int waveCount = (*parameterValues)[0] / 10;
	float waveSize = (*parameterValues)[1] / 30.0;

	float waveAdjust = cos(waveCount * theta) * waveSize * (phi + PI/2);

	v->position.x = (r + waveAdjust) * cos(theta) * cos(phi);
	v->position.y = (r + waveAdjust) * sin(theta) * cos(phi);
	v->position.z = (r + waveAdjust) * sin(phi) + (totalR - totalR * sin(totalPhi - PI / 2))/2.0;

}

void Mesher :: applyPotTransform(Vertex* v, vector<int>* parameterValues)
{
	// compute theta within a rollup, starting with pickup geometry
	float theta = PI * v->position.x / 5.0;

	// Deform into a spline-based vase
	float body_radius = (*parameterValues)[0] * 0.03 + 1.0;
	float lip_radius = (*parameterValues)[1] * 0.03 + 0.1;
	float radius = 2.0 * splineVal(lip_radius, body_radius, lip_radius, (v->position.z - -5.0)/10.0);

	if (radius < 1.0)
	{
		v->position.x = (radius - v->position.y * radius) * cos(theta);
		v->position.y = (radius - v->position.y * radius) * sin(theta);
	}
	else
	{
		v->position.x = (radius - v->position.y / radius) * cos(theta);
		v->position.y = (radius - v->position.y / radius) * sin(theta);
	}
}

void Mesher :: applyVaseTransform(Vertex* v, vector<int>* parameterValues)
{
	// compute theta within a rollup, starting with pickup geometry
	float theta = PI * v->position.x / 5.0;

	// Deform into a spline-based vase
	float lip_radius = (*parameterValues)[0] * 0.03 + 0.5; 
	float body_radius = (*parameterValues)[1] * 0.03 + 1.0; 
	float twist_theta = PI * v->position.z * (*parameterValues)[2] / 500.0; 
	float radius = 2.0 * splineVal(0.1, body_radius, 0.5, lip_radius, (v->position.z - -5.0)/10.0);

	if (radius < 1.0)
	{
		v->position.x = (radius - v->position.y * radius) * cos(theta + twist_theta); 
		v->position.y = (radius - v->position.y * radius) * sin(theta + twist_theta); 
	}
	else
	{
		v->position.x = (radius - v->position.y / radius) * cos(theta + twist_theta); 
		v->position.y = (radius - v->position.y / radius) * sin(theta + twist_theta); 
	}
}

float Mesher :: asymptoteVal(float s, float t)
{
	return 1 - (s / (t + s)); 
}

void Mesher :: applyTumblerTransform(Vertex* v, vector<int>* parameterValues)
{
	// compute theta within a rollup, starting with pickup geometry
	float theta = PI * v->position.x / 5.0;

	// Deform into a spline-based vase
	float radius = (((*parameterValues)[0] + 40) * 0.05) * asymptoteVal(0.01 * ((*parameterValues)[1]*0.1 + 1), (v->position.z - -5.0)/10.0);

	if (radius < 1.0)
	{
		v->position.x = (radius - v->position.y * radius) * cos(theta); 
		v->position.y = (radius - v->position.y * radius) * sin(theta); 
	}
	else
	{
		v->position.x = (radius - v->position.y / radius) * cos(theta); 
		v->position.y = (radius - v->position.y / radius) * sin(theta); 
	}
}

Vertex Mesher :: applyTransforms(Vertex v, vector<PullPlan*>* ancestors, vector<int>* ancestorIndices)
{
	for (int i = ancestors->size() - 2; i >= 0; --i)
	{
		applyMoveAndResizeTransform(&v, (*ancestors)[i], (*ancestorIndices)[i]);
		applyTwistTransform(&v,(*ancestors)[i]);
	}
	return v;
}

void Mesher :: meshPickupCasingSlab(Geometry* geometry, Color* color, float y, float thickness)
{
	unsigned int slabResolution = 100;

	//need to know first vertex position so we can transform 'em all later
	uint32_t first_vert = geometry->vertices.size();
	//need to remember the first triangle so we can tag it later
	uint32_t first_triangle = geometry->triangles.size();

	Vector2f p;
	vector< Vector2f > points;
		
	for (unsigned int i = 0; i < slabResolution; ++i)
	{
		p.x = 4.999 - 9.998* i / (slabResolution-1);
		p.y = y + thickness + 0.01;
		points.push_back(p);
	}
	for (unsigned int i = 0; i < slabResolution; ++i)
	{
		p.x = -4.999 + 9.998 * i / (slabResolution-1);
		p.y = y + -thickness - 0.01;
		points.push_back(p);
	}

	//Generate verts:
	vector< Vector3ui > tris;
	if (tris.empty() && points.size() >= 3) {
		tris.resize(points.size() - 2);
		for (unsigned int i = 0; i < tris.size(); ++i) {
			tris[i].c[0] = 0;
			tris[i].c[1] = i+1;
			tris[i].c[2] = i+2;
		}
	}

	unsigned int base = geometry->vertices.size();
	for (unsigned int i = 0; i < slabResolution; ++i)
	{
		for (unsigned int j = 0; j < points.size(); ++j)
		{
			Point p;
			p.xy = points[j];
			if (i == 0)
				p.z = -4.999;
			else
				p.z = -4.999 + 9.998 * i / (slabResolution-1);
			Point n;
			//This is a terrible normal estimate, but I guess it gets re-estimated anyway.
			n.x = p.x;
			n.y = p.y;
			n.z = 0.0f;
			geometry->vertices.push_back(Vertex(p,n));
		}
	}
	//Generate triangles linking them:
	for (unsigned int i = 0; i + 1 < slabResolution; ++i)
	{
		for (unsigned int j = 0; j < slabResolution-1; ++j)
		{
			uint32_t p1 = base + i * 2 * slabResolution + j;
			uint32_t p2 = base + (i+1) * 2 * slabResolution + j;
			uint32_t p3 = base + i * 2 * slabResolution + j+1;
			uint32_t p4 = base + (i+1) * 2 * slabResolution + j+1;
			// Four points that define a (non-flat) quad are used
			// to create two triangles.
			geometry->triangles.push_back(Triangle(p2, p1, p4));
			geometry->triangles.push_back(Triangle(p1, p3, p4));
		}
		for (unsigned int j = slabResolution; j < 2 * slabResolution - 1; ++j)
		{
			uint32_t p1 = base + i * 2 * slabResolution + j;
			uint32_t p2 = base + (i+1) * 2 * slabResolution + j;
			uint32_t p3 = base + i * 2 * slabResolution + j+1;
			uint32_t p4 = base + (i+1) * 2 * slabResolution + j+1;
			// Four points that define a (non-flat) quad are used
			// to create two triangles.
			geometry->triangles.push_back(Triangle(p2, p1, p4));
			geometry->triangles.push_back(Triangle(p1, p3, p4));
		}
	}

	assert(geometry->valid());

	base = geometry->vertices.size();
	for (unsigned int j = 0; j < points.size(); ++j)
	{
		Point p;
		p.xy = points[j];
		p.z = -4.99;

		Point n;
		n.x = 0.0; n.y = 0.0; n.z = -1.0;
		geometry->vertices.push_back(Vertex(p, n));
	}
	for (unsigned int j = 0; j < slabResolution; ++j)
	{
		uint32_t p1 = base + j;
		uint32_t p2 = base + j+1;
		uint32_t p3 = base + 2 * slabResolution-1 - j;
		uint32_t p4 = base + 2 * slabResolution-1 - (j+1);
		// Four points that define a (non-flat) quad are used
		// to create two triangles.
		geometry->triangles.push_back(Triangle(p2, p1, p4));
		geometry->triangles.push_back(Triangle(p1, p3, p4));
	}
	
	base = geometry->vertices.size();
	for (unsigned int j = 0; j < points.size(); ++j)
	{
		Point p;
		p.xy = points[j];
		p.z = 4.99;

		Point n;
		n.x = 0.0; n.y = 0.0; n.z = 1.0;
		geometry->vertices.push_back(Vertex(p, n));
	}
	for (unsigned int j = 0; j < slabResolution-1; ++j)
	{
		uint32_t p1 = base + j;
		uint32_t p2 = base + j+1;
		uint32_t p3 = base + 2 * slabResolution-1 - j;
		uint32_t p4 = base + 2 * slabResolution-1 - (j+1);
		// Four points that define a (non-flat) quad are used
		// to create two triangles.
		geometry->triangles.push_back(Triangle(p1, p2, p4));
		geometry->triangles.push_back(Triangle(p1, p4, p3));
	}
	
	assert(geometry->valid());

	geometry->groups.push_back(Group(first_triangle, geometry->triangles.size() - first_triangle, 
		first_vert, geometry->vertices.size() - first_vert, color, true, -1));
}

/*
The cane should have length between 0.0 and 1.0 and is scaled up by a factor of 5.
*/
void Mesher :: meshPolygonalBaseCane(Geometry* geometry, vector<PullPlan*>* ancestors, vector<int>* ancestorIndices, 
	Color* color, int mandatedShape, float offset, float length, float radius, bool ensureVisible, uint32_t group_tag)
{
	if (color->a < 0.0001 && !ensureVisible)
		return;

	unsigned int angularResolution = MIN(MAX(TOTAL_ANGULAR_RESOLUTION / totalCaneLength, 10), 25);
	unsigned int axialResolution = MIN(MAX(TOTAL_AXIAL_RESOLUTION / totalCaneLength * length, 10), 100);
	
	//need to know first vertex position so we can transform 'em all later
	uint32_t first_vert = geometry->vertices.size();
	//need to remember the first triangle so we can tag it later
	uint32_t first_triangle = geometry->triangles.size();

	Vector2f p;
	vector< Vector2f > points;
	switch (mandatedShape) // comes from plan or template of parent plan
	{
		case CIRCLE_SHAPE:
			for (unsigned int i = 0; i < angularResolution; ++i)
			{
				p.x = cos(TWO_PI * i / angularResolution);
				p.y = sin(TWO_PI * i / angularResolution);
				points.push_back(p);
			}
			break;
		case SQUARE_SHAPE: 
			for (unsigned int i = 0; i < angularResolution / 4; ++i)
			{
				p.x = 1.0;
				p.y = -1.0 + 8.0 * i / angularResolution;
				points.push_back(p);
			}
			for (unsigned int i = 0; i < angularResolution / 4; ++i)
			{
				p.x = 1.0 - 8.0 * i / angularResolution;
				p.y = 1.0;
				points.push_back(p);
			}
			for (unsigned int i = 0; i < angularResolution / 4; ++i)
			{
				p.x = -1.0;
				p.y = 1.0 - 8.0 * i / angularResolution;
				points.push_back(p);
			}
			for (unsigned int i = 0; i < angularResolution / 4; ++i)
			{
				p.x = -1.0 + 8.0 * i / angularResolution;
				p.y = -1.0;
				points.push_back(p);
			}
			break;
	}

	// scale for radius 
	for (unsigned int i = 0; i < points.size(); ++i) {
		points[i].x *= radius;
		points[i].y *= radius;
	}

	// Create wall vertices row by row
	unsigned int base = geometry->vertices.size();
	for (unsigned int i = 0; i < axialResolution; ++i)
	{
		for (unsigned int j = 0; j < points.size(); ++j)
		{
			Point p;
			p.xy = points[j];
			if (i == 0)
				p.z = 5.0 * offset;
			else
				p.z = 5.0 * length * i / (axialResolution-1);
			Point n;
			//This is a terrible normal estimate, but I guess it gets re-estimated anyway.
			n.x = p.x;
			n.y = p.y;
			n.z = 0.0f;
			geometry->vertices.push_back(Vertex(p,n));
		}
	}
	//Generate triangles linking wall vertices in adjacent rows
	for (unsigned int i = 0; i + 1 < axialResolution; ++i)
	{
		for (unsigned int j = 0; j < points.size(); ++j)
		{
			uint32_t p1 = base + i * points.size() + j;
			uint32_t p2 = base + (i+1) * points.size() + j;
			uint32_t p3 = base + i * points.size() + (j+1) % points.size();
			uint32_t p4 = base + (i+1) * points.size() + (j+1) % points.size();
			// Four points that define a (non-flat) quad are used
			// to create two triangles.
			geometry->triangles.push_back(Triangle(p2, p1, p4));
			geometry->triangles.push_back(Triangle(p2, p1, p4));
			geometry->triangles.push_back(Triangle(p1, p3, p4));
		}
	}

	// Build top and bottom triangles (two concentric rings) 
	vector< Vector3ui > layer1Tris;
	layer1Tris.resize(points.size());
	for (unsigned int i = 0; i < points.size(); ++i) {
		layer1Tris[i].c[0] = -1;
		layer1Tris[i].c[1] = i;
		layer1Tris[i].c[2] = (i+1) % points.size();
	}

	vector< Vector3ui > layer2Tris;
	layer2Tris.resize(2*points.size());
	for (unsigned int i = 0; i < points.size(); ++i) {
		layer2Tris[2*i].c[0] = i-points.size();
		layer2Tris[2*i].c[1] = i;
		layer2Tris[2*i].c[2] = (i+1) % points.size();
		layer2Tris[2*i+1].c[0] = i-points.size();
		layer2Tris[2*i+1].c[1] = (i+1) % points.size();
		layer2Tris[2*i+1].c[2] = (i+1) % points.size() - points.size();
	}

	/*
	Build top and bottom walls (disks) of cylinder by generating
	vertices and linking them with just-constructed triangle lists
	*/
	for (int side = 0; side <= 1; ++side) 
	{
		float z;
		int o1, o2;

		if (side)
		{
			o1 = 1;
			o2 = 2;
			z = 5.0 * length;
		}
		else
		{
			o1 = 2;
			o2 = 1;
			z = 5.0 * offset;
		}

		Point n;
		n.x = 0.0; 
		n.y = 0.0; 
		n.z = (side ? 1.0:-1.0);

		// throw down first layer of points and triangles (central layer)
		Point p;
		p.x = p.y = 0.0;
		p.z = z;
		geometry->vertices.push_back(Vertex(p, n));

		uint32_t base = geometry->vertices.size();
		for (unsigned int j = 0; j < points.size(); ++j)
		{
			p.xy = points[j];
			p.x *= 0.5; // fit this ring of points inside full cylinder cap
			p.y *= 0.5;
			p.z = z;
			geometry->vertices.push_back(Vertex(p, n));
		}
		for (unsigned int j = 0; j < layer1Tris.size(); ++j)
		{
			geometry->triangles.push_back(Triangle(base + layer1Tris[j].c[0], base + layer1Tris[j].c[o1], base + layer1Tris[j].c[o2]));
		}

		// throw down second layer of points and triangles (outer ring layer)
		base = geometry->vertices.size();
		for (unsigned int j = 0; j < points.size(); ++j)
		{
			p.xy = points[j];
			p.z = z;
			geometry->vertices.push_back(Vertex(p, n));
		}
		for (unsigned int j = 0; j < layer2Tris.size(); ++j)
		{
			geometry->triangles.push_back(Triangle(base + layer2Tris[j].c[0], base + layer2Tris[j].c[o1], base + layer2Tris[j].c[o2]));
		}
	}
	assert(geometry->valid());

	// Actually do the transformations on the basic canonical cylinder mesh
	for (uint32_t v = first_vert; v < geometry->vertices.size(); ++v)
	{
		geometry->vertices[v] = applyTransforms(geometry->vertices[v], ancestors, ancestorIndices);
	}
	geometry->groups.push_back(Group(first_triangle, geometry->triangles.size() - first_triangle, 
		first_vert, geometry->vertices.size() - first_vert, color, ensureVisible, group_tag));
}

void Mesher :: generateMesh(Piece* piece, Geometry* geometry, vector<PullPlan*>* ancestors, vector<int>* ancestorIndices)
{
	if (piece == NULL)
		return;

	geometry->clear();
	
	generateMesh(piece->pickup, geometry, false, ancestors, ancestorIndices);		

	switch (piece->getTemplate()->type)
	{
		case VASE_PIECE_TEMPLATE:
			for (uint32_t i = 0; i < geometry->vertices.size(); ++i)
				applyVaseTransform(&(geometry->vertices[i]), &(piece->getTemplate()->parameterValues));
			break;
		case TUMBLER_PIECE_TEMPLATE:
			for (uint32_t i = 0; i < geometry->vertices.size(); ++i)
				applyTumblerTransform(&(geometry->vertices[i]), &(piece->getTemplate()->parameterValues));
			break;
		case BOWL_PIECE_TEMPLATE:
			for (uint32_t i = 0; i < geometry->vertices.size(); ++i)
				applyBowlTransform(&(geometry->vertices[i]), &(piece->getTemplate()->parameterValues));
			break;
		case POT_PIECE_TEMPLATE:
			for (uint32_t i = 0; i < geometry->vertices.size(); ++i)
				applyPotTransform(&(geometry->vertices[i]), &(piece->getTemplate()->parameterValues));
			break;
		case WAVY_PLATE_PIECE_TEMPLATE:
			for (uint32_t i = 0; i < geometry->vertices.size(); ++i)
				applyWavyPlateTransform(&(geometry->vertices[i]), &(piece->getTemplate()->parameterValues));
			break;

	}	

}

void Mesher :: generateMesh(PickupPlan* pickup, Geometry *geometry, bool ignoreCasing, vector<PullPlan*>* ancestors, vector<int>* ancestorIndices)
{
	if (pickup == NULL)
		return;

	geometry->clear();
	for (unsigned int i = 0; i < pickup->subs.size(); ++i)
	{
		ancestors->clear();
		ancestorIndices->clear();
		generateMesh(pickup->subs[i].plan, pickup->subs[i].plan->getOutermostCasingShape(), geometry, 
			ancestors, ancestorIndices, 0.0, pickup->subs[i].length, ignoreCasing, i); 

		for (uint32_t g = 0; g < geometry->groups.size(); ++g)
		{
			if (geometry->groups[g].tag == i)
			{
				Group* subpullGroup = &(geometry->groups[g]);

				// Apply transformation to only these vertices
				for (uint32_t v = subpullGroup->vertex_begin; 
					v < subpullGroup->vertex_begin + subpullGroup->vertex_size; ++v)
				{
					applyPickupTransform(&(geometry->vertices[v]), &(pickup->subs[i]));
				}
			}
		}
	}
	meshPickupCasingSlab(geometry, pickup->overlayColorBar->getOutermostCasingColor(), 0.0, pickup->subs[0].width*2.5 + 0.01);
	if (pickup->underlayColorBar->getOutermostCasingColor()->a > 0.001) 
		meshPickupCasingSlab(geometry, pickup->underlayColorBar->getOutermostCasingColor(), pickup->subs[0].width*2.5 + 0.01 + 0.06, 0.05);
}


void Mesher :: generateMesh(Piece* piece, Geometry* geometry)
{
	totalCaneLength = computeTotalCaneLength(piece);
	vector<PullPlan*> ancestors;
	vector<int> ancestorIndices;
	generateMesh(piece, geometry, &ancestors, &ancestorIndices);
	geometry->compute_normals_from_triangles();
}

void Mesher :: generateMesh(PickupPlan* plan, Geometry* geometry)
{
	totalCaneLength = computeTotalCaneLength(plan);
	vector<PullPlan*> ancestors;
	vector<int> ancestorIndices;
	generateMesh(plan, geometry, true, &ancestors, &ancestorIndices);
	geometry->compute_normals_from_triangles();
}

void Mesher :: generatePullMesh(PullPlan* plan, Geometry* geometry)
{
	totalCaneLength = computeTotalCaneLength(plan);
	vector<PullPlan*> ancestors;
	vector<int> ancestorIndices;
	if (plan->getTemplateType() == AMORPHOUS_BASE_PULL_TEMPLATE)
		generateMesh(plan, CIRCLE_SHAPE, geometry, &ancestors, &ancestorIndices, 0.0, 2.0, true);
	else
		generateMesh(plan, plan->getOutermostCasingShape(), geometry, &ancestors, &ancestorIndices, 0.0, 2.0, true);

	// Make skinnier to more closely mimic the canes found in pickups
	for (uint32_t v = 0; v < geometry->vertices.size(); ++v)
	{
		applyResizeTransform(&(geometry->vertices[v]), 0.5);
	}
	geometry->compute_normals_from_triangles();
}

void Mesher :: generateColorMesh(PullPlan* plan, Geometry* geometry)
{
	totalCaneLength = computeTotalCaneLength(plan);
	vector<PullPlan*> ancestors;
	vector<int> ancestorIndices;
	if (plan->getTemplateType() == AMORPHOUS_BASE_PULL_TEMPLATE)
		generateMesh(plan, CIRCLE_SHAPE, geometry, &ancestors, &ancestorIndices, 0.0, 2.0, true);
	else
		generateMesh(plan, plan->getOutermostCasingShape(), geometry, &ancestors, &ancestorIndices, 0.0, 2.0, true);
	geometry->compute_normals_from_triangles();
}

/*
generateMesh() is the top-level function for turning a cane into
a geometry that can be rendered. As generateMesh() is called recursively,
the transforms array is filled with with the transformations encountered at each node. When a
leaf is reached, these transformations are used to generate a complete mesh
for the leaf node.
*/
void Mesher :: generateMesh(PullPlan* plan, int mandatedShape, Geometry *geometry, vector<PullPlan*>* ancestors, 
	vector<int>* ancestorIndices, float offset, float length, bool ensureVisible, int groupIndex)
{
	int passGroupIndex;

	if (plan == NULL)
		return;

	ancestors->push_back(plan); 
	// for the outermost casing, use the shape suggestion from your parent, as you might 
	// have an AMORPHOUS_SHAPE and need it specified for you. Even if you don't the shape
	// suggestion matches your shape, so it's still ok to use.
	meshPolygonalBaseCane(geometry, ancestors, ancestorIndices, plan->getOutermostCasingColor(), 
		mandatedShape, offset, length, 1.0, ensureVisible, groupIndex);
	for (unsigned int i = 0; i < plan->getCasingCount(); ++i) {
		meshPolygonalBaseCane(geometry, ancestors, ancestorIndices, plan->getCasingColor(i), 
			plan->getCasingShape(i), offset, length, plan->getCasingThickness(i), ensureVisible, groupIndex);
	}
	ancestors->pop_back();

	// Make recursive calls depending on the type of the current node
	ancestors->push_back(plan);

	if (plan->isBase())
	{
		if (groupIndex == -1)
			passGroupIndex = 0;
		else
			passGroupIndex = groupIndex;
	}
	else 
	{
		for (unsigned int i = 0; i < plan->subs.size(); ++i)
		{
			if (groupIndex == -1)
				passGroupIndex = i;
			else
				passGroupIndex = groupIndex;

			ancestorIndices->push_back(i);
			generateMesh(plan->subs[i].plan, plan->subs[i].shape, geometry, ancestors, 
				ancestorIndices, offset - 0.001, length + 0.001, false, passGroupIndex);
			ancestorIndices->pop_back();
		}
	}
	ancestors->pop_back();
}



