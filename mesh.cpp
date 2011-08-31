
#include "mesh.h"

#include <set>
#include <map>

using std::set;
using std::map;
using std::make_pair;


Point rotate(Point pre, float theta)
{
	Point post;

	post.x = cos(theta) * pre.x - sin(theta) * pre.y;
	post.y = sin(theta) * pre.x + cos(theta) * pre.y;

	return post;
}

void applyFlattenSubcaneTransform(Geometry* geometry, int subcane, Cane* transformNode)
{
	// Find group for subcane
	Group* subcaneGroup;
	for (uint32_t g = 0; g < geometry->groups.size(); ++g)
		if (geometry->groups[g].tag == (uint32_t) subcane)
		{
			subcaneGroup = &(geometry->groups[g]);

			// Apply transformation to only these vertices
			for (uint32_t v = subcaneGroup->vertex_begin; v < subcaneGroup->vertex_begin + subcaneGroup->vertex_size; ++v)
			{
				applyFlattenTransform(&(geometry->vertices[v]), transformNode);
			}
		}
	geometry->compute_normals_from_triangles();
}

void applyFlattenTransform(Geometry* geometry, Cane* transformNode)
{
	for (uint32_t v = 0; v < geometry->vertices.size(); ++v)
	{
		applyFlattenTransform(&(geometry->vertices[v]), transformNode);
	}
	geometry->compute_normals_from_triangles();
}

void applyFlattenTransform(Vertex* v, Cane* transformNode)
{
	float rectangleRatio = transformNode->amts[0];
	float rectangleTheta = transformNode->amts[1];
	float flatness = transformNode->amts[2];

	Point vert;
	vert.x = v->position.x;
	vert.y = v->position.y;
	Point vertRectCoords = rotate(vert, -rectangleTheta);

	float radius = length(vertRectCoords.xy);
	float theta = atan2(vertRectCoords.y, vertRectCoords.x);
	if (theta < 0)
		theta += 2*PI;
	float arc_length = radius * theta;

	/*
	The vertex lies on a circle that is mapped to a rectangle
	with equal perimeter. The rectangleRatio is width/height.
	*/
	float rectY = PI * radius / (1.0 + rectangleRatio);
	float rectX = rectY * rectangleRatio;

	// Use half width/height
	rectY /= 2.0;
	rectX /= 2.0;

	Point goalRectangle;
	if (arc_length < rectY)
	{
		goalRectangle.x = rectX;
		goalRectangle.y = arc_length;
	}
	else if ((arc_length - rectY) < 2 * rectX)
	{
		goalRectangle.x = rectX - (arc_length - rectY);
		goalRectangle.y = rectY;
	}
	else if ((arc_length - rectY - 2 * rectX) < 2 * rectY)
	{
		goalRectangle.x = -rectX;
		goalRectangle.y = rectY - (arc_length - rectY - 2 * rectX);
	}
	else if ((arc_length - rectY - 2 * rectX - 2 * rectY) < 2 * rectX)
	{
		goalRectangle.x = -rectX + (arc_length - rectY - 2 * rectX - 2 * rectY);
		goalRectangle.y = -rectY;
	}
	else
	{
		goalRectangle.x = rectX;
		goalRectangle.y = -rectY + (arc_length - rectY - 2 * rectX - 2 * rectY - 2 * rectX);
	}

	// Transform back to normal coordinates from rectangle coordinates.
	goalRectangle = rotate(goalRectangle, rectangleTheta);
	v->position.x = flatness * goalRectangle.x + vert.x * (1 - flatness);
	v->position.y = flatness * goalRectangle.y + vert.y * (1 - flatness);
}

void applyMoveTransform(Vertex* v, Cane* parentNode, int subcane)
{
	Point locationInParent = parentNode->subcaneLocations[subcane];

	// Z location
	float preTheta = atan2(v->position.y, v->position.x);
	float postTheta = preTheta + locationInParent.z;
	float r = length(v->position.xy);
	v->position.x = r * cos(postTheta);
	v->position.y = r * sin(postTheta);

	// XY location
	v->position.x += locationInParent.x;
	v->position.y += locationInParent.y;
}

void applyPartialMoveTransform(Geometry* geometry, Cane* parentNode, int subcane, float deltaX, float deltaY, float deltaZ)
{
	// Find group for subcane
	Group* subcaneGroup;
	for (uint32_t g = 0; g < geometry->groups.size(); ++g)
		if (geometry->groups[g].tag == (uint32_t) subcane)
		{
			subcaneGroup = &(geometry->groups[g]);

			// Apply transformation to only these vertices
			for (uint32_t v = subcaneGroup->vertex_begin; v < subcaneGroup->vertex_begin + subcaneGroup->vertex_size; ++v)
			{
				// Z location
				Point subcaneCenter;
				subcaneCenter.x = parentNode->subcaneLocations[subcane].x - deltaX;
				subcaneCenter.y = parentNode->subcaneLocations[subcane].y - deltaY;
				float preTheta = atan2(geometry->vertices[v].position.y - subcaneCenter.y, geometry->vertices[v].position.x - subcaneCenter.x);
				float postTheta = preTheta + deltaZ;
				float r = sqrt((geometry->vertices[v].position.x - subcaneCenter.x) * (geometry->vertices[v].position.x - subcaneCenter.x) + (geometry->vertices[v].position.y - subcaneCenter.y) * (geometry->vertices[v].position.y - subcaneCenter.y));
				geometry->vertices[v].position.x = r * cos(postTheta) + subcaneCenter.x;
				geometry->vertices[v].position.y = r * sin(postTheta) + subcaneCenter.y;

				// XY location
				geometry->vertices[v].position.x += deltaX;
				geometry->vertices[v].position.y += deltaY;
			}
		}
	geometry->compute_normals_from_triangles();
}

void applyPullSubcaneTransform(Geometry* geometry, int subcane, Cane* transformNode)
{
	// Find group for subcane
	Group* subcaneGroup;
	for (uint32_t g = 0; g < geometry->groups.size(); ++g)
		if (geometry->groups[g].tag == (uint32_t) subcane)
		{
			subcaneGroup = &(geometry->groups[g]);

			// Apply transformation to only these vertices
			for (uint32_t v = subcaneGroup->vertex_begin; v < subcaneGroup->vertex_begin + subcaneGroup->vertex_size; ++v)
			{
				applyPullTransform(&(geometry->vertices[v]), transformNode);
			}
		}
	geometry->compute_normals_from_triangles();
}

void applyPullTransform(Geometry* geometry, Cane* transformNode)
{
	for (uint32_t v = 0; v < geometry->vertices.size(); ++v)
	{
		applyPullTransform(&(geometry->vertices[v]), transformNode);
	}
	geometry->compute_normals_from_triangles();
}

void applyPullTransform(Vertex* v, Cane* transformNode)
{
	float twist = transformNode->amts[0];
	float stretch = transformNode->amts[1];

	// Apply twist first
	float preTheta = atan2(v->position.y, v->position.x);
	float r = length(v->position.xy);
	float transformTheta = stretch * twist * v->position.z;
	float postTheta = preTheta + transformTheta;
	v->position.x = r * cos(postTheta);
	v->position.y = r * sin(postTheta);

	// Then apply stretch
	v->position.x /= sqrt(stretch);
	v->position.y /= sqrt(stretch);
	v->position.z *= stretch;
}

/*
This function applies a sequence of transformations to a 3D point.
This is used during the meshing process to determine the location of
a vertex after it has been moved via the transformations described
by its ancestors in the cane DAG.
*/
Vertex applyTransforms(Vertex v, Cane** ancestors, int ancestorCount, bool fullTransforms)
{
	/*
	The transformations are applied back to front to match how they
	are loaded into the array. Because the transform array is created by
	a depth-first traversal of the cane DAG, transforms lower in the DAG
	(i.e. closer to the leaves) are added later. However, they
	represent the first operations done on the cane, so need to be applied
	first.
	*/
	for (int i = ancestorCount - 1; i >= 0; --i)
	{
		/*
		Each cane node has a type and an amount.
		Depending upon the type, the amount fields
		take on different meanings. For instance, a twist transform
		uses amts[0] to mean the magnitude of the twist.
		just a single real-valued parameter).
		The BUNDLE_CANETYPE is an exception, in that it simply uses
		the location of the subcane to determine how to move the points.
		*/
		switch (ancestors[i]->type)
		{
		case BUNDLE_CANETYPE:
		{
			// Find where subcane lives and apply translation
			// to move it to this location
			int subcaneIndex = -1;
			for (int j = 0; j < ancestors[i]->subcaneCount; ++j)
			{
				if (ancestors[i]->subcanes[j] == ancestors[i+1])
				{
					subcaneIndex = j;
					break;
				}
			}
			applyMoveTransform(&v, ancestors[i], subcaneIndex);
			break;
		}
		case PULL_CANETYPE:
			// amts[0] describes the width-to-height ratio of the goal rectangle
			// amts[1] describes the orientation w.r.t global XY
			// amts[2] describes how close of an approximation to the rectangle is achieved
			if (fullTransforms || i > 0)
				applyPullTransform(&v, ancestors[i]);
			//TODO: normal transform
			break;
		case FLATTEN_CANETYPE:
			if (fullTransforms || i > 0)
				applyFlattenTransform(&v, ancestors[i]);
			break;
		default: // BASE_CIRCLE_CANETYPE
			break;
		}
	}
	return v;
}

/*
This function is used in the process of reparameterizing the mesh after
a stretch, to avoid losing vertices when the cane is cut down to just
the z-region between 0 and 1.
*/
float computeTotalStretch(Cane** ancestors, int ancestorCount)
{
	float totalStretch;
	int i;

	totalStretch = 1;
	for (i = 0; i < ancestorCount; ++i)
	{
		if (ancestors[i]->type == PULL_CANETYPE)
			totalStretch *= ancestors[i]->amts[1];
	}

	return totalStretch;
}

typedef map< Vector2ui, Vector2ui > EdgeMap;
typedef set< Vector2ui > EdgeSet;


void meshPolygonalBaseCane(Geometry* geometry, float meshHeight, Cane** ancestors, int ancestorCount,
	int resolution, Cane* group_cane, uint32_t group_tag, bool fullTransforms)
{
	unsigned int axialResolution;
	float total_stretch = computeTotalStretch(ancestors, ancestorCount) / meshHeight;

	switch (resolution)
	{
		case LOW_RESOLUTION:
			axialResolution = LOW_AXIAL_RESOLUTION;
			break;
		case HIGH_RESOLUTION:
			axialResolution = HIGH_AXIAL_RESOLUTION;
			break;
	}

	//need to know first vertex position so we can transform 'em all later
	uint32_t first_vert = geometry->vertices.size();
	//need to remember the first triangle so we can tag it later
	uint32_t first_triangle = geometry->triangles.size();

	/*
	Draw the walls of the polygon. Note that the z location is
	adjusted by the total stretch experienced by the cane so that
	the z values range between 0 and 1.
	*/

	//Generate verts:
	vector< Vector2f > const &points = group_cane->shape.getVertices();
	vector< Vector3ui > tris = group_cane->shape.tris;
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
				p.z = ((float) i) / ((axialResolution-1) * total_stretch);
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
	for (int side = 0; side <= 1; ++side) {
		float z = (side?1.0:0.0);
		float nz = (side?1.0:-1.0);
		uint32_t base = geometry->vertices.size();
		for (unsigned int j = 0; j < points.size(); ++j)
		{
			Point p;
			p.xy = points[j];
			p.z = z / total_stretch;
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
		geometry->vertices[v] = applyTransforms(geometry->vertices[v], ancestors, ancestorCount, fullTransforms);
	}
	geometry->compute_normals_from_triangles();
	geometry->groups.push_back(Group(first_triangle, geometry->triangles.size() - first_triangle, first_vert, geometry->vertices.size() - first_vert, group_cane, group_tag));
}

/*
generateMesh() is the top-level function for turning a cane into
a geometry that can be rendered. As generateMesh() is called recursively,
the transforms array is filled with with the transformations encountered at each node. When a
leaf is reached, these transformations are used to generate a complete mesh
for the leaf node.
*/
void generateMesh(Cane* c, Geometry *geometry, float meshHeight, Cane** ancestors, int* ancestorCount, int resolution, bool fullTransforms, int groupIndex)
{
	int i, passGroupIndex;

	if (c == NULL)
		return;

	// Make recursive calls depending on the type of the current node
	ancestors[*ancestorCount] = c;
	*ancestorCount += 1;

	if (c->type == BASE_POLYGONAL_CANETYPE)
	{
		if (groupIndex == -1)
			passGroupIndex = 0;
		else
			passGroupIndex = groupIndex;

		meshPolygonalBaseCane(geometry, meshHeight, ancestors, *ancestorCount, resolution, c, passGroupIndex, fullTransforms);
	}
	else if (c->type == BUNDLE_CANETYPE)
	{
		for (i = 0; i < c->subcaneCount; ++i)
		{
			if (groupIndex == -1)
				passGroupIndex = i;
			else
				passGroupIndex = groupIndex;
			generateMesh(c->subcanes[i], geometry, meshHeight, ancestors, ancestorCount, resolution, fullTransforms, passGroupIndex);
		}
	}
	else
	{
		for (i = 0; i < c->subcaneCount; ++i)
		{
			generateMesh(c->subcanes[i], geometry, meshHeight, ancestors, ancestorCount, resolution, fullTransforms, groupIndex);
		}
	}
	*ancestorCount -= 1;
}



