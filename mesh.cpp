
#include "mesh.h"

#include <set>
#include <map>

using std::set;
using std::map;
using std::make_pair;


Mesher :: Mesher()
{
        Color color;
        color.r = color.g = color.b = 1.0;
        color.a = 0.2;
        circleCasing = new PullPlan(CIRCLE_BASE_TEMPLATE, true, color);
        squareCasing = new PullPlan(SQUARE_BASE_TEMPLATE, true, color);
	trigTableSize = 1000;	
	for (int i = 0; i < trigTableSize; ++i)
	{
		cosTable.push_back(cos(TWO_PI * i / trigTableSize));
	}
	for (int i = 0; i < trigTableSize; ++i)
	{
		sinTable.push_back(sin(TWO_PI * i / trigTableSize));
	}
}

float Mesher :: tableCos(float theta)
{
	if (theta < 0)
		theta = TWO_PI - fmod(-theta, TWO_PI);	
	else
		theta = fmod(theta, TWO_PI);
	return cosTable[trigTableSize * (theta / TWO_PI)];
}

float Mesher :: tableSin(float theta)
{
	if (theta < 0)
		theta = TWO_PI - fmod(-theta, TWO_PI);	
	else
		theta = fmod(theta, TWO_PI);
	return sinTable[trigTableSize * (theta / TWO_PI)];
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
	for (unsigned int i = 0; i < plan->subplans.size(); ++i)
	{
		total += computeTotalCaneLength(plan->subplans[i]) * plan->getTemplate()->subpulls[i]->length / 2.0;
	}
	return total;
}

float Mesher :: computeTotalCaneLength(PullPlan* plan)
{
	if (plan->isBase)
		return 1.0;
	
	float total = 0.0;
	for (unsigned int i = 0; i < plan->subplans.size(); ++i)
	{
		total += computeTotalCaneLength(plan->subplans[i]);
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

void Mesher :: applyMoveAndResizeTransform(Vertex* v, PullPlan* parentNode, int subplan)
{
	SubpullTemplate* subTemp = &(parentNode->getTemplate()->subpulls[subplan]);
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
	float twist = transformNode->twist;

	// Apply twist
	float preTheta = atan2(v->position.y, v->position.x);
	float r = length(v->position.xy);
	float transformTheta = twist / 10.0 * v->position.z;
	float postTheta = preTheta + transformTheta;
	v->position.x = r * tableCos(postTheta);
	v->position.y = r * tableSin(postTheta);
}

void Mesher :: applyPickupTransform(Vertex* v, SubpickupTemplate* spt)
{
	// Shrink length to correct length
	v->position.z = v->position.z * spt->length / 2.0;

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
	//v->position.y = v->position.y * 0.2;

}

void Mesher :: applyBowlTransform(Vertex* v, vector<int>* parameterValues)
{
	int radius = (*parameterValues)[0];
	int twist = (*parameterValues)[1];

	// Do a rollup
	// send x value to theta value 
	// -5.0 goes to -PI, 5.0 goes to PI
	// everything gets a base radius of 5.0
	float theta = PI * v->position.x / 5.0 + PI * v->position.z * twist / 500.0;

	float totalR = 4.0 + radius * 0.1;
	float totalPhi = 10.0 / totalR;

	float r = totalR - v->position.y;
	float phi = ((v->position.z - -5.0) / 10.0) * totalPhi - PI/2;

	v->position.x = r * tableCos(theta) * tableCos(phi);
	v->position.y = r * tableSin(theta) * tableCos(phi);
	v->position.z = r * tableSin(phi) + (totalR - totalR * tableSin(totalPhi - PI / 2))/2.0;
}

void Mesher :: applyTumblerTransform(Vertex* v, vector<int>* parameterValues)
{
	int width = (*parameterValues)[0];
	int taper = (*parameterValues)[1];

	// Do a rollup
	// send x value to theta value 
	// -5.0 goes to -PI, 5.0 goes to PI
	// everything gets a base radius of 5.0
	float theta = PI * v->position.x / 5.0;
	float r = 5.0 / PI - v->position.y;

	// Shape into a tumbler
	float cutoff = -5.0 + 5.0 / PI + width * 0.05;
	if (v->position.z < cutoff)
	{
		float R = v->position.z - -5.0;
		float offset = r - 5.0 / PI + 0.1;
		v->position.x = R * tableCos(theta);
		v->position.y = R * tableSin(theta);
		v->position.z = cutoff + offset; 
	}
	else
	{
		float R = cutoff - -5.0 + (r - 5.0 / PI) + taper * 0.01 * (v->position.z - cutoff);
		v->position.x = R * tableCos(theta);
		v->position.y = R * tableSin(theta);
	}

	// recenter the object about the origin
	v->position.z -= (5.0 + cutoff) / 2;

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

typedef map< Vector2ui, Vector2ui > EdgeMap;
typedef set< Vector2ui > EdgeSet;

/*
start and end determine the length of cane created...they should be between 0.0 and 1.0.
The resulting cane has length between 0.0 and 10.0, i.e. it is scaled by a factor of 10.
*/
void Mesher :: meshPolygonalBaseCane(Geometry* geometry, vector<PullPlan*>* ancestors, vector<int>* ancestorIndices, PullPlan* plan,
	float start, float end, uint32_t group_tag)
{
	unsigned int angularResolution = MIN(MAX(400 / totalCaneLength, 6), 60);
	unsigned int axialResolution = MIN(MAX(1500 / totalCaneLength * (end - start), 5), 100);
	
	//need to know first vertex position so we can transform 'em all later
	uint32_t first_vert = geometry->vertices.size();
	//need to remember the first triangle so we can tag it later
	uint32_t first_triangle = geometry->triangles.size();

	Vector2f p;
	vector< Vector2f > points;
	switch (plan->getTemplate()->shape)
	{
		case CIRCLE_SHAPE:
			for (unsigned int i = 0; i < angularResolution; ++i)
			{
				p.x = tableCos(TWO_PI * i / angularResolution);
				p.y = tableSin(TWO_PI * i / angularResolution);
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
	vector< vector< unsigned int > > loops;
	{ //make edge loops, given tris:
		EdgeMap next_edge;
		//First, make it easy to walk around edges:
		for (vector< Vector3ui >::const_iterator tri = tris.begin(); tri != tris.end(); ++tri) {
			for (unsigned int i = 0; i < 3; ++i) {
				unsigned int a = tri->c[i];
				unsigned int b = tri->c[(i+1)%3];
				unsigned int c = tri->c[(i+2)%3];
				//do edges incident to vertex b:
				//next_edge[(a,b)] = (b,c)
				assert(!next_edge.count(make_vector(a,b)));
				next_edge.insert(make_pair(make_vector(a,b),make_vector(b,c)));
			}
		}
		EdgeSet outside_edges;
		for (EdgeMap::const_iterator n = next_edge.begin(); n != next_edge.end(); ++n) {
			Vector2ui e = n->first;
			//any edge without a reverse is an outside edge:
			if (!next_edge.count(make_vector(e[1],e[0]))) {
				outside_edges.insert(e);
			}
		}
		while (!outside_edges.empty()) {
			assert(outside_edges.size() >= 3);

			Vector2ui seed = *outside_edges.begin();
			outside_edges.erase(outside_edges.begin());
			vector< unsigned int > loop;
			Vector2ui edge = seed;
			while (1) {
				//Add edge to loop:
				loop.push_back(edge[0]);
				//Walk to next edge in triangle:
				EdgeMap::iterator f = next_edge.find(edge);
				assert(f != next_edge.end());
				edge = f->second;
				//Follow adjacent triangles:
				while ((f = next_edge.find(make_vector(edge[1],edge[0]))) != next_edge.end()) {
					edge = f->second;
				}
				//Should darn well lead to an outside edge:
				EdgeSet::iterator o = outside_edges.find(edge);
				if (o == outside_edges.end()) {
					//we looped.
					assert(edge == seed);
					break;
				} else {
					outside_edges.erase(o);
				}
			}
			assert(loop.size() >= 3);

			loops.push_back(loop);
		} //while there are still edge loops to walk.
	}
	for (vector< vector< unsigned int > >::const_iterator loop = loops.begin(); loop != loops.end(); ++loop)
	{
		unsigned int base = geometry->vertices.size();
		for (unsigned int i = 0; i < axialResolution; ++i)
		{
			for (unsigned int j = 0; j < loop->size(); ++j)
			{
				Point p;
				p.xy = points[(*loop)[j]];
				p.z = 10.0 * (start + (end - start) * i / (axialResolution-1));
				Point n;
				//This is a terrible normal estimate, but I guess it gets re-estimated anyway.
				n.x = p.x;
				n.y = p.y;
				n.z = 0.0f;
				geometry->vertices.push_back(Vertex(p,n));
			}
		}
		//Generate triangles linking them:
		for (unsigned int i = 0; i + 1 < axialResolution; ++i)
		{
			for (unsigned int j = 0; j < loop->size(); ++j)
			{
				uint32_t p1 = base + i * loop->size() + j;
				uint32_t p2 = base + (i+1) * loop->size() + j;
				uint32_t p3 = base + i * loop->size() + (j+1) % loop->size();
				uint32_t p4 = base + (i+1) * loop->size() + (j+1) % loop->size();
				// Four points that define a (non-flat) quad are used
				// to create two triangles.
				geometry->triangles.push_back(Triangle(p2, p1, p4));
				geometry->triangles.push_back(Triangle(p1, p3, p4));
			}
		}
	} //for (loops)
	assert(geometry->valid());

	/*
	Draw the polygon bottom, then top.
	The mesh uses a set of n-2 triangles with a common vertex
	to draw a regular n-gon.
	*/
	for (int side = 0; side <= 1; ++side) 
	{
		float z;
		if (side)
			z = 10.0 * end;
		else
			z = 10.0 * start;
		float nz = (side ? 1.0:-1.0);
		uint32_t base = geometry->vertices.size();
		for (unsigned int j = 0; j < points.size(); ++j)
		{
			Point p;
			p.xy = points[j];
			p.z = z;

			Point n;
			n.x = 0.0; n.y = 0.0; n.z = nz;
			geometry->vertices.push_back(Vertex(p, n));
		}
		if (side)
		{
			for (unsigned int j = 0; j < tris.size(); ++j)
			{
				geometry->triangles.push_back(Triangle(base + tris[j].c[0], base + tris[j].c[1], base + tris[j].c[2]));
			}
		}
		else
		{
			for (unsigned int j = 0; j < tris.size(); ++j)
			{
				geometry->triangles.push_back(Triangle(base + tris[j].c[0], base + tris[j].c[2], base + tris[j].c[1]));
			}
		}
	}
	assert(geometry->valid());

	for (uint32_t v = first_vert; v < geometry->vertices.size(); ++v)
	{
		geometry->vertices[v] = applyTransforms(geometry->vertices[v], ancestors, ancestorIndices);
	}
	geometry->compute_normals_from_triangles();
	geometry->groups.push_back(Group(first_triangle, geometry->triangles.size() - first_triangle, first_vert, geometry->vertices.size() - first_vert, plan, group_tag));
}

void Mesher :: generateMesh(Piece* piece, Geometry* geometry, vector<PullPlan*>* ancestors, vector<int>* ancestorIndices)
{
	if (piece == NULL)
		return;

	geometry->clear();
	
	generateMesh(piece->pickup, geometry, ancestors, ancestorIndices);		
	
	Vertex* v;
        for (uint32_t i = 0; i < geometry->vertices.size(); ++i)
        {
		v = &(geometry->vertices[i]);
		switch (piece->getTemplate()->type)
		{
			case TUMBLER_TEMPLATE:
				applyTumblerTransform(v, &(piece->getTemplate()->parameterValues));
				break;
			case BOWL_TEMPLATE:
				applyBowlTransform(v, &(piece->getTemplate()->parameterValues));
				break;
		}
	}	
	geometry->compute_normals_from_triangles();
}

void Mesher :: generateMesh(PickupPlan* plan, Geometry *geometry, vector<PullPlan*>* ancestors, vector<int>* ancestorIndices)
{
	if (plan == NULL)
		return;

	geometry->clear();
	for (unsigned int i = 0; i < plan->subplans.size(); ++i)
	{
		ancestors->clear();
		ancestorIndices->clear();
		generateMesh(plan->subplans[i], geometry, ancestors, ancestorIndices, 0.0, 
			plan->getTemplate()->subpulls[i]->length / 2.0, true, i); 

		for (uint32_t g = 0; g < geometry->groups.size(); ++g)
		{
			if (geometry->groups[g].tag == i)
			{
				Group* subpullGroup = &(geometry->groups[g]);

				// Apply transformation to only these vertices
				for (uint32_t v = subpullGroup->vertex_begin; 
					v < subpullGroup->vertex_begin + subpullGroup->vertex_size; ++v)
				{
					applyPickupTransform(&(geometry->vertices[v]), plan->getTemplate()->subpulls[i]);
				}
			}
		}
	}
	geometry->compute_normals_from_triangles();
}


/*
generateMesh() is the top-level function for turning a cane into
a geometry that can be rendered. As generateMesh() is called recursively,
the transforms array is filled with with the transformations encountered at each node. When a
leaf is reached, these transformations are used to generate a complete mesh
for the leaf node.
*/
void Mesher :: generateMesh(PullPlan* plan, Geometry *geometry, vector<PullPlan*>* ancestors, vector<int>* ancestorIndices, 
	float start, float end, bool addCasing, int groupIndex)
{
	int passGroupIndex;

	if (plan == NULL)
		return;

	// Deal with casing first
	if (addCasing && !plan->isBase)
	{
		PullPlan* casingPlan = NULL;
		switch (plan->getTemplate()->shape)
		{
			case CIRCLE_SHAPE:
				casingPlan = circleCasing;
				break;	
			case SQUARE_SHAPE:
				casingPlan = squareCasing;
				break;	
		}		
		
		if (casingPlan != NULL)
		{	
			ancestors->push_back(casingPlan); 
			meshPolygonalBaseCane(geometry, ancestors, ancestorIndices, casingPlan, start - 0.01, 
				end + 0.01, groupIndex);
			ancestors->pop_back();
		}
	}

	// Make recursive calls depending on the type of the current node
	ancestors->push_back(plan);

	if (plan->isBase)
	{
		if (groupIndex == -1)
			passGroupIndex = 0;
		else
			passGroupIndex = groupIndex;
		
		meshPolygonalBaseCane(geometry, ancestors, ancestorIndices, plan, start, end, passGroupIndex);
	}
	else 
	{
		for (unsigned int i = 0; i < plan->subplans.size(); ++i)
		{
			if (groupIndex == -1)
				passGroupIndex = i;
			else
				passGroupIndex = groupIndex;

			ancestorIndices->push_back(i);
			generateMesh(plan->subplans[i], geometry, ancestors, ancestorIndices, start, end, false, passGroupIndex);
			ancestorIndices->pop_back();
		}
	}
	ancestors->pop_back();
}



