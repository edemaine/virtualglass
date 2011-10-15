
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
	post.z = pre.z;

	return post;
}

Vertex rotateX(Vertex pre, float theta)
{
	Vertex post;

	post.position.x = pre.position.x;
	post.position.y = cos(theta) * pre.position.y - sin(theta) * pre.position.z;
	post.position.z = sin(theta) * pre.position.y + cos(theta) * pre.position.z;

	post.normal.x = pre.normal.x;
	post.normal.y = cos(theta) * pre.normal.y + sin(theta) * pre.normal.z;
	post.normal.z = sin(theta) * pre.normal.y - cos(theta) * pre.normal.z;

	return post;
}

void applyMoveTransform(Vertex* v, PullPlan* parentNode, int subplan)
{
	Point locationInParent = parentNode->getTemplate()->locations[subplan];

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

void applyPullTransform(Geometry* geometry, PullPlan* transformNode)
{
	for (uint32_t v = 0; v < geometry->vertices.size(); ++v)
	{
		applyPullTransform(&(geometry->vertices[v]), transformNode);
	}
	geometry->compute_normals_from_triangles();
}

void applyPullTransform(Vertex* v, PullPlan* transformNode)
{
	float twist = transformNode->getTwist();
	float stretch = transformNode->getLength();

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

Vertex applyTransforms(Vertex v, PullPlan** ancestors, int ancestorCount)
{
	for (unsigned int i = ancestorCount - 1; i > 0; --i)
	{
		int subcaneIndex = -1;
		for (unsigned int j = 0; j < ancestors[i]->getTemplate()->locations.size(); ++j)
		{
			if (ancestors[i]->getSubplans()[j] == ancestors[i+1])
			{
				subcaneIndex = j;
				break;
			}
		}
		applyMoveTransform(&v, ancestors[i], subcaneIndex);
		applyPullTransform(&v, ancestors[i]);
	}
	return v;
}


typedef map< Vector2ui, Vector2ui > EdgeMap;
typedef set< Vector2ui > EdgeSet;

void meshPolygonalBaseCane(Geometry* geometry, PullPlan** ancestors, int ancestorCount, PullPlan* plan, uint32_t group_tag)
{
	unsigned int angularResolution = 15;
	unsigned int axialResolution = 60;

	//need to know first vertex position so we can transform 'em all later
	uint32_t first_vert = geometry->vertices.size();
	//need to remember the first triangle so we can tag it later
	uint32_t first_triangle = geometry->triangles.size();

	// Tiny offset for avoiding collinear triangles in different canes
	float random_z_offset = 0.001 * rand() / RAND_MAX;

	/*
	Draw the walls of the polygon. Note that the z location is
	adjusted by the total stretch experienced by the cane so that
	the z values range between 0 and 1.
	*/

	Vector2f p;
	vector< Vector2f > points;
	for (unsigned int i = 0; i < angularResolution; ++i)
	{
		p[0] = cos(2 * PI * i / angularResolution);
		p[1] = sin(2 * PI * i / angularResolution);

		points.push_back(p);
	} 

	Vector3ui t;
	vector< Vector3ui > tris;
	for (unsigned int i = 2; i < angularResolution; ++i)
	{
		t[0] = 0;
		t[1] = 1;
		t[2] = i;
		tris.push_back(t);
	}

	//Generate verts:
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
				p.z = ((float) i) / ((axialResolution-1)) + random_z_offset;
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
		float z = (side ? 1.0:0.0);
		float nz = (side ? 1.0:-1.0);
		uint32_t base = geometry->vertices.size();
		for (unsigned int j = 0; j < points.size(); ++j)
		{
			Point p;
			p.xy = points[j];
			// Put last point in general position
			p.z = z + random_z_offset;

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
		geometry->vertices[v] = applyTransforms(geometry->vertices[v], ancestors, ancestorCount);
	}
	geometry->compute_normals_from_triangles();
	geometry->groups.push_back(Group(first_triangle, geometry->triangles.size() - first_triangle, first_vert, geometry->vertices.size() - first_vert, plan, group_tag));
}


/*
generateMesh() is the top-level function for turning a cane into
a geometry that can be rendered. As generateMesh() is called recursively,
the transforms array is filled with with the transformations encountered at each node. When a
leaf is reached, these transformations are used to generate a complete mesh
for the leaf node.
*/
void generateMesh(PullPlan* plan, Geometry *geometry, PullPlan** ancestors, int* ancestorCount, int groupIndex)
{
	int passGroupIndex;

	if (plan == NULL)
		return;

	// Make recursive calls depending on the type of the current node
	ancestors[*ancestorCount] = plan;
	*ancestorCount += 1;

	if (plan->isBase())
	{
		if (groupIndex == -1)
			passGroupIndex = 0;
		else
			passGroupIndex = groupIndex;
		
		meshPolygonalBaseCane(geometry, ancestors, *ancestorCount, plan, passGroupIndex);
	}
	else 
	{
		for (unsigned int i = 0; i < plan->getSubplans().size(); ++i)
		{
			if (groupIndex == -1)
				passGroupIndex = i;
			else
				passGroupIndex = groupIndex;
			generateMesh(plan->getSubplans()[i], geometry, ancestors, ancestorCount, passGroupIndex);
		}
	}
	*ancestorCount -= 1;
}



