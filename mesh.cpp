
#include "mesh.h"

void applyFlattenTransform(Vertex* v, float rectangleRatio, float rectangleTheta, float flatness)
{
	Point p_r;
	p_r.x = cos(-rectangleTheta) * v->position.x - sin(-rectangleTheta) * v->position.y;
	p_r.y = sin(-rectangleTheta) * v->position.x + cos(-rectangleTheta) * v->position.y;

	float pt_radius = length(p_r.xy);
	float pt_theta = atan2(p_r.y, p_r.x);
	if (pt_theta < 0)
		pt_theta += 2*PI;
	float arc_length = pt_radius * pt_theta;

	// We use a boundary-preserving circle to rectangle transformation
	float rect_y = PI * pt_radius / (1.0 + rectangleRatio);
	float rect_x = rect_y * rectangleRatio;
	rect_y /= 2.0;
	rect_x /= 2.0;
	if (arc_length < rect_y)
	{
		p_r.x = rect_x;
		p_r.y = arc_length;
	}
	else
	{
		arc_length -= rect_y;
		if (arc_length < 2 * rect_x)
		{
			p_r.x = rect_x - arc_length;
			p_r.y = rect_y;
		}
		else
		{
			arc_length -= 2 * rect_x;
			if (arc_length < 2 * rect_y)
			{
				p_r.x = -rect_x;
				p_r.y = rect_y - arc_length;
			}
			else
			{
				arc_length -= 2 * rect_y;
				if (arc_length < 2 * rect_x)
				{
					p_r.x = -rect_x + arc_length;
					p_r.y = -rect_y;
				}
				else
				{
					arc_length -= 2 * rect_x;
					p_r.x = rect_x;
					p_r.y = -rect_y + arc_length;
				}
			}
		}
	}

	v->position.x = flatness * (cos(rectangleTheta) * p_r.x
								- sin(rectangleTheta) * p_r.y) + v->position.x * (1-flatness);
	v->position.y = flatness * (sin(rectangleTheta) * p_r.x
								+ cos(rectangleTheta) * p_r.y) + v->position.y * (1-flatness);
	//TODO: normal transform
}

void applyBundleTransform(Vertex* v, Point location)
{
	v->position.x += location.x;
	v->position.y += location.y;
	v->position.z += location.z; //NEW
}

void applyPullTransform(Vertex* v, float twistAmount, float stretchAmount)
{
	// Apply twist first
	float theta = atan2(v->position.y, v->position.x);
	float r = length(v->position.xy);
	theta += stretchAmount * twistAmount * v->position.z;
	v->position.x = r * cos(theta);
	v->position.y = r * sin(theta);

	// Then apply stretch
	v->position.x /= sqrt(stretchAmount);
	v->position.y /= sqrt(stretchAmount);
	v->position.z *= stretchAmount;
}

/*
This function applies a sequence of transformations to a 3D point.
This is used during the meshing process to determine the location of
a vertex after it has been moved via the transformations described
by its ancestors in the cane DAG.
*/
Vertex applyTransforms(Vertex v, Cane** ancestors, int ancestorCount)
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
			// Find where subcane lives and apply translation
			// to move it to this location
			int subcaneIndex;
			for (int j = 0; j < ancestors[i]->subcaneCount; ++j)
			{
				if (ancestors[i]->subcanes[j] == ancestors[i+1])
				{
					subcaneIndex = j;
					break;
				}
			}
			applyBundleTransform(&v, ancestors[i]->subcaneLocations[subcaneIndex]);
			break;
			// amts[0] is twist, amts[1] is stretch
		case PULL_CANETYPE:
			applyPullTransform(&v, ancestors[i]->amts[0], ancestors[i]->amts[1]);
			//TODO: normal transform
			break;
			// amts[0] describes the width-to-height ratio of the goal rectangle
			// amts[1] describes the orientation w.r.t global XY
			// amts[2] describes how close of an approximation to the rectangle is achieved
		case FLATTEN_CANETYPE:
			applyFlattenTransform(&v, ancestors[i]->amts[0], ancestors[i]->amts[1],
								  ancestors[i]->amts[2]);
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

/*
meshCircularBaseCane() creates a mesh for a radius 1, length 1 cylindrical piece of cane,
and applies a sequences of transforms (coming from a depth-first traversal of the cane ending
with this leaf base cane). The triangles are added to the end of the array passed in.

The resolution refers to the dual resolution modes used by the GUI, and the actual number of
triangles for these resolutions are set in constants.h
*/
float meshCircularBaseCane(Geometry *geometry, Cane** ancestors,
	int ancestorCount, int resolution, Cane *group_cane, uint32_t group_tag, float radius, bool computeRadius)
{
	unsigned int angularResolution, axialResolution;
	float total_stretch, transformedRadius;

	switch (resolution)
	{
	case LOW_RESOLUTION:
		angularResolution = LOW_ANGULAR_RESOLUTION;
		axialResolution = LOW_AXIAL_RESOLUTION;
		break;
	case HIGH_RESOLUTION:
		angularResolution = HIGH_ANGULAR_RESOLUTION;
		axialResolution = HIGH_AXIAL_RESOLUTION;
		break;
	default:
		exit(1);
	}

	//DEBUG: total_stretch shortened... why is the top cap missing?
	total_stretch = computeTotalStretch(ancestors, ancestorCount);

	//need to know first vertex position so we can transform 'em all later
	uint32_t first_vert = geometry->vertices.size();
	//need to remember the first triangle so we can tag it later
	uint32_t first_triangle = geometry->triangles.size();

	/*
	Draw the walls of the cylinder. Note that the z location is
	adjusted by the total stretch experienced by the cane so that
	the z values range between 0 and 1.
	*/
	//Generate verts:
	for (unsigned int i = 0; i < axialResolution; ++i)
	{
		for (unsigned int j = 0; j < angularResolution; ++j)
		{
			Point p;
			Point n;

			p.x = radius * cos(2 * PI * ((float) j) / angularResolution);
			p.y = radius * sin(2 * PI * ((float) j) / angularResolution);
			p.z = ((float) i) / ((axialResolution-1) * total_stretch);
			n.x = p.x;
			n.y = p.y;
			n.z = 0.0f;
			geometry->vertices.push_back(Vertex(p,n));
		}
	}
	//Generate triangles linking them:
	for (unsigned int i = 0; i + 1 < axialResolution; ++i)
	{
		for (unsigned int j = 0; j < angularResolution; ++j)
		{
			uint32_t p1 = first_vert + i * angularResolution + j;
			uint32_t p2 = first_vert + (i+1) * angularResolution + j;
			uint32_t p3 = first_vert + i * angularResolution + (j+1) % angularResolution;
			uint32_t p4 = first_vert + (i+1) * angularResolution + (j+1) % angularResolution;
			// Four points that define a (non-flat) quad are used
			// to create two triangles.
			geometry->triangles.push_back(Triangle(p2, p1, p4));
			//was: tmp_t.v1 = p2; tmp_t.v2 = p1; tmp_t.v3 = p4;

			geometry->triangles.push_back(Triangle(p1, p3, p4));
			//was: tmp_t.v1 = p1; tmp_t.v2 = p3; tmp_t.v3 = p4;
		}
	}
	assert(geometry->valid());

	/*
	Draw the cylinder bottom, then top.
	The mesh uses a set of n-2 triangles with a common vertex
	to draw a regular n-gon.
	*/
	for (int side = 0; side <= 1; ++side) {
		float z = (side?1.0:0.0);
		float nz = (side?1.0:-1.0);
		uint32_t base = geometry->vertices.size();
		for (unsigned int j = 0; j < angularResolution; ++j)
		{
			Point p;
			p.x = radius * cos(2 * PI * ((float) j) / angularResolution);
			p.y = radius * sin(2 * PI * ((float) j) / angularResolution);
			p.z = z / total_stretch;
			Point n;
			n.x = 0.0; n.y = 0.0; n.z = nz;
			geometry->vertices.push_back(Vertex(p, n));
		}
		if (side)
		{
			for (unsigned int j = 1; j + 1 < angularResolution; ++j)
			{
				geometry->triangles.push_back(Triangle(base, base + j, base + j + 1));
			}
		}
		else
		{
			for (unsigned int j = 1; j + 1 < angularResolution; ++j)
			{
				geometry->triangles.push_back(Triangle(base, base + j + 1, base + j));
			}
		}
	}
	assert(geometry->valid());

	for (uint32_t v = first_vert; v < geometry->vertices.size(); ++v)
	{
		geometry->vertices[v] = applyTransforms(geometry->vertices[v], ancestors, ancestorCount);
	}
	geometry->compute_normals_from_triangles();
	geometry->groups.push_back(Group(first_triangle, 
		geometry->triangles.size() - first_triangle, group_cane, group_tag));

	if (!computeRadius)
		return 0.0;

	// Compute radius of resulting cane
	transformedRadius = 0;
	for (uint32_t v = first_vert; v < geometry->vertices.size(); ++v)
	{
		transformedRadius = MAX(transformedRadius, geometry->vertices[v].position.x * geometry->vertices[v].position.x + geometry->vertices[v].position.y * geometry->vertices[v].position.y); 
	}
	transformedRadius = sqrt(transformedRadius);

	return transformedRadius;
}


/*
generateMesh() is the top-level function for turning a cane into
a geometry that can be rendered. As generateMesh() is called recursively,
the transforms array is filled with with the transformations encountered at each node. When a
leaf is reached, these transformations are used to generate a complete mesh
for the leaf node.
*/
float generateMesh(Cane* c, Geometry *geometry, Cane** ancestors, int* ancestorCount,
	int resolution, bool casing, bool computeRadius, int coloringIndex)
{
	int i, passCasing, passColoringIndex;
	float radius;

	if (c == NULL)
		return 0.0;

	// Make recursive calls depending on the type of the current node
	ancestors[*ancestorCount] = c;
	*ancestorCount += 1;
	if (c->type == BASE_CIRCLE_CANETYPE)
	{
		radius = meshCircularBaseCane(geometry, ancestors, *ancestorCount,
			resolution, c, coloringIndex, 1.0, computeRadius);
	}
	else
	{
		passCasing = (casing && c->type != CASING_CANETYPE);
		for (i = 0; i < c->subcaneCount; ++i)
		{
			if (coloringIndex == -1)
				passColoringIndex = i;
			else 
				passColoringIndex = coloringIndex;

			radius = MAX(generateMesh(c->subcanes[i], geometry, ancestors, ancestorCount,
				resolution, passCasing, computeRadius, passColoringIndex), radius);
		}
		if (casing && c->type == CASING_CANETYPE)
		{
			radius = MAX(meshCircularBaseCane(geometry, ancestors, *ancestorCount,
				resolution, c, coloringIndex, c->amts[0], computeRadius), radius);
		}
	}
	*ancestorCount -= 1;

	return radius;
}



