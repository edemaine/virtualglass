
#include <vector>
#include <map>
#include <utility>
#include "constants.h"
#include "glasscolor.h"
#include "pullplan.h"
#include "pickupplan.h"
#include "piece.h"
#include "subpulltemplate.h"
#include "mesh.h"
#include "spline.h"

using namespace MeshInternal;

using std::vector;
using std::pair;
using std::map;

void generateMesh(Piece* piece, Geometry* pieceGeometry, Geometry* pickupGeometry, unsigned int quality)
{
	if (pickupGeometry != NULL)
	{
		pickupGeometry->clear();
		generateMesh(piece->pickup, pickupGeometry, true, quality);
		pickupGeometry->compute_normals_from_triangles();
	}

	if (pieceGeometry != NULL)
	{
		pieceGeometry->clear();
		generateMesh(piece->pickup, pieceGeometry, false, quality);
		applyPieceTransform(pieceGeometry, piece);
		casePiece(pieceGeometry, piece);
		pieceGeometry->compute_normals_from_triangles();
	}
}

void generateMesh(PullPlan* plan, Geometry* geometry, unsigned int quality)
{
	geometry->clear();

	vector<ancestor> ancestors;
	recurseMesh(plan, geometry, ancestors, 2.0, quality, true);			

	// Make skinnier to more closely mimic the canes found in pickups
	for (uint32_t v = 0; v < geometry->vertices.size(); ++v)
	{
		applyResizeTransform(geometry->vertices[v], 0.5);
	}
	geometry->compute_normals_from_triangles();
}

void generateMesh(GlassColor* gc, Geometry* geometry, unsigned int quality)
{
	PullPlan dummyPlan(PullTemplate::BASE_CIRCLE);
	dummyPlan.setOutermostCasingColor(gc);

	geometry->clear();
	vector<ancestor> ancestors;
	recurseMesh(&dummyPlan, geometry, ancestors, 2.0, quality, true);
	geometry->compute_normals_from_triangles();
}


namespace MeshInternal
{

void applyResizeTransform(Vertex& v, float scale)
{
	// Adjust diameter
	v.position.x *= scale; 
	v.position.y *= scale; 
}

void applyTwistTransform(Vertex& v, float twist)
{
	// Adjust location in parent for twist
	float r = sqrt(v.position.x * v.position.x + v.position.y * v.position.y);
	float preTheta = atan2(v.position.y, v.position.x);
	float postTheta = twist * TWO_PI * v.position.z / 10.0 + preTheta;
	v.position.x += (r * cos(postTheta) - r * cos(preTheta));
	v.position.y += (r * sin(postTheta) - r * sin(preTheta));
}

// Does move, resize, and twist
void applySubplanTransform(Vertex& v, Point2D location)
{
	v.position.x += location.x;
	v.position.y += location.y;
}

void applyPickupTransform(Vertex& v, SubpickupTemplate& spt)
{
	// Shrink width to correct width
	v.position.x *= spt.width * 2.5; 
	v.position.y *= spt.width * 2.5; 

	// Change orientation if needed
	float tmp;
	switch (spt.orientation)
	{
		case HORIZONTAL_PICKUP_CANE_ORIENTATION:
			tmp = v.position.x;
			v.position.x = v.position.z;
			v.position.z = -tmp;
			break;
		case VERTICAL_PICKUP_CANE_ORIENTATION:
			break;
		case MURRINE_PICKUP_CANE_ORIENTATION:
			tmp = v.position.y;
			v.position.y = v.position.z;
			v.position.z = -tmp;
			break;
	}

	// Offset by location
	v.position.x = v.position.x + spt.location.x * 5.0;
	v.position.z = v.position.z + spt.location.y * 5.0;
	v.position.y = v.position.y + spt.location.z * 5.0;
}

void applyPieceTransform(Vertex& v, float twist, Spline& spline)
{
	float diskTheta = PI * v.position.x / 5.0 + twist * TWO_PI * v.position.z / 10.0;
	float caneLoc = v.position.y;
	float t = (v.position.z - -5.0) / 10.0;

	Point2D p1 = spline.get(t);
	Point2D p2 = spline.get(t + 0.01);
	// remove artifacts at piece base due to bevel-like angling of pickup slab
	float fix = 0.05;
	if (t < fix)
		p2.y = 1.0 / fix * (fix - t) * p1.y + 1.0 / fix * t * p2.y;

	float delta_theta = atan2(p2.y - p1.y, p2.x - p1.x);
	delta_theta -= PI/2;

	v.position.x = (p1.x + cos(delta_theta) * -caneLoc) * cos(diskTheta);
	v.position.y = (p1.x + cos(delta_theta) * -caneLoc) * sin(diskTheta);
	v.position.z = p1.y + -caneLoc * sin(delta_theta);
}

void casePiece(Geometry* geometry, Piece* piece)
{
	// base thickness of casing off of representative (first) cane in pickup
	float thickness;
	if (piece->pickup->subs[0].orientation == MURRINE_PICKUP_CANE_ORIENTATION)
		thickness = piece->pickup->subs[0].length*2.5;	
	else
		thickness = piece->pickup->subs[0].width*2.5;
	
	// mesh the slab and apply the casing
	unsigned int verticesStart = geometry->vertices.size();
	meshPickupCasingSlab(geometry, piece->pickup->casingGlassColor->getColor(), 0.0, thickness);
	for (uint32_t i = verticesStart; i < geometry->vertices.size(); ++i)
		applyPieceTransform(geometry->vertices[i], 0.0 /* NO TWIST */, piece->spline);

	//for now we don't do these, but the intent is to reintroduce soon
	//meshPickupCasingSlab(geometry, pickup->underlayGlassColor->getColor(), thickness + 0.1, 0.05);
	//meshPickupCasingSlab(geometry, pickup->overlayGlassColor->getColor(), -(thickness + 0.1), 0.05);
}

void applyPieceTransform(Geometry* geometry, Piece* piece)
{
	for (uint32_t i = 0; i < geometry->vertices.size(); ++i)
		applyPieceTransform(geometry->vertices[i], piece->twist, piece->spline);
}

void applySubplanTransforms(Vertex& v, vector<ancestor>& ancestors, bool isRotInvar)
{
	for (unsigned int i = ancestors.size() - 1; i < ancestors.size(); --i)
	{
		ancestor& a = ancestors[i];
		SubpullTemplate& subTemp = a.parent->subs[a.child];
		if (isRotInvar)
			applyTwistTransform(v, -a.parent->twist);
		applyResizeTransform(v, subTemp.diameter / 2.0);
		applySubplanTransform(v, subTemp.location);
		applyTwistTransform(v, a.parent->twist);
	}
}

void meshPickupCasingSlab(Geometry* geometry, Color color, float y, float thickness)
{
	unsigned int slabResolution = 50;

	uint32_t first_vert = geometry->vertices.size();
	uint32_t first_triangle = geometry->triangles.size();

	Point2D p;
	vector<Point2D> points;

	// generate a basis set of points		
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

	// generate pull vertex set for slabs
	unsigned int base = geometry->vertices.size();
	for (unsigned int i = 0; i < slabResolution; ++i)
	{
		for (unsigned int j = 0; j < points.size(); ++j)
		{
			Point3D p;
			p.xy = points[j];
			if (i == 0)
				p.z = -5.001;
			else
				p.z = -5.001 + 10.002 * i / (slabResolution-1);
			Point3D n;
			//This is a terrible normal estimate, but I guess it gets re-estimated anyway.
			n.x = p.x;
			n.y = p.y;
			n.z = 0.0f;
			geometry->vertices.push_back(Vertex(p,n));
		}
	}
	// mesh slabs
	for (unsigned int i = 0; i + 1 < slabResolution; ++i)
	{
		// inner slab
		for (unsigned int j = 0; j < slabResolution-1; ++j)
		{
			uint32_t p1 = base + i * 2 * slabResolution + j;
			uint32_t p2 = base + (i+1) * 2 * slabResolution + j;
			uint32_t p3 = base + i * 2 * slabResolution + j+1;
			uint32_t p4 = base + (i+1) * 2 * slabResolution + j+1;

			geometry->triangles.push_back(Triangle(p2, p1, p4));
			geometry->triangles.push_back(Triangle(p1, p3, p4));
		}
		// outer slab
		for (unsigned int j = slabResolution; j < 2 * slabResolution - 1; ++j)
		{
			uint32_t p1 = base + i * 2 * slabResolution + j;
			uint32_t p2 = base + (i+1) * 2 * slabResolution + j;
			uint32_t p3 = base + i * 2 * slabResolution + j+1;
			uint32_t p4 = base + (i+1) * 2 * slabResolution + j+1;

			geometry->triangles.push_back(Triangle(p2, p1, p4));
			geometry->triangles.push_back(Triangle(p1, p3, p4));
		}
	}

	// generate vertex set for bottom strip
	base = geometry->vertices.size();
	for (unsigned int j = 0; j < points.size(); ++j)
	{
		Point3D p;
		p.xy = points[j];
		p.z = -4.99;

		Point3D n;
		n.x = 0.0; n.y = 0.0; n.z = -1.0;
		geometry->vertices.push_back(Vertex(p, n));
	}
	// mesh bottom strip connecting the two slabs
	for (unsigned int j = 0; j < slabResolution; ++j)
	{
		uint32_t p1 = base + j;
		uint32_t p2 = base + j+1;
		uint32_t p3 = base + 2 * slabResolution-1 - j;
		uint32_t p4 = base + 2 * slabResolution-1 - (j+1);

		geometry->triangles.push_back(Triangle(p2, p1, p4));
		geometry->triangles.push_back(Triangle(p1, p3, p4));
	}

	// generate vertex set for top strip connecting the two slabs
	base = geometry->vertices.size();
	for (unsigned int j = 0; j < points.size(); ++j)
	{
		Point3D p;
		p.xy = points[j];
		p.z = 4.99;

		Point3D n;
		n.x = 0.0; n.y = 0.0; n.z = 1.0;
		geometry->vertices.push_back(Vertex(p, n));
	}
	// mesh top strip connecting the two slabs	
	for (unsigned int j = 0; j < slabResolution-1; ++j)
	{
		uint32_t p1 = base + j;
		uint32_t p2 = base + j+1;
		uint32_t p3 = base + 2 * slabResolution-1 - j;
		uint32_t p4 = base + 2 * slabResolution-1 - (j+1);

		geometry->triangles.push_back(Triangle(p1, p2, p4));
		geometry->triangles.push_back(Triangle(p1, p4, p3));
	}

	color.a = MAX(color.a, 0.1);
	geometry->groups.push_back(Group(first_triangle, geometry->triangles.size() - first_triangle, 
		first_vert, geometry->vertices.size() - first_vert, color));
}

void getTemplatePoints(vector<Point2D>& points, unsigned int angularResolution, 
	enum GeometricShape shape, float radius)
{

	// force angularResolution to be the closest multiple of 4
	// so square mesh has consistent resolution along each side
	assert(angularResolution % 4 == 0);

	points.clear();
	// all shapes start at angle -PI/4 (i.e. x = 1.0, y = -1.0)
	// this consistency is necessary for meshBaseCasing() which stitches the tops
	// of them together	
	Point2D p;
	switch (shape)
	{
		case CIRCLE_SHAPE:
			for (unsigned int i = 0; i < angularResolution; ++i)
			{
				p.x = cos(TWO_PI * i / angularResolution - PI/4);
				p.y = sin(TWO_PI * i / angularResolution - PI/4);
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
	for (unsigned int i = 0; i < points.size(); ++i) 
	{
		points[i].x *= radius;
		points[i].y *= radius;
	}

} 

void meshCylinderWall(Geometry* geometry, enum GeometricShape shape, float length, float radius, 
	unsigned int angularResolution, unsigned int axialResolution)
{
	assert(angularResolution % 4 == 0);
	vector<Point2D> points;
	getTemplatePoints(points, angularResolution, shape, radius);

	// Create wall vertices row by row
	unsigned int base = geometry->vertices.size();
	for (unsigned int i = 0; i < axialResolution; ++i)
	{
		for (unsigned int j = 0; j < points.size(); ++j)
		{
			Point3D p;
			p.xy = points[j];
			if (i == 0)
				p.z = 0.0;
			else
				p.z = 5.0 * length * i / (axialResolution-1);
			Point3D n;
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
			geometry->triangles.push_back(Triangle(p1, p3, p4));
		}
	}
} 

// goals for resolutions with respect to quality:
// quality ranges from 1 to 10: 1 is low, 10 is high
// 5 gives something decent with mild artifacts
// 1 gives something really ugly but low triangle count
// 10 gives something with no artifacts
unsigned int computeAngularResolution(float radius, unsigned int quality)
{
	// standard "full" cane radius is 1.0
	// this function should return a multiple of 4
	unsigned int r = radius * 6 * quality;
	return MIN(MAX((r / 4) * 4, 4), 20); 
}

unsigned int computeAxialResolution(float length, float twist, unsigned int quality)
{
	float twistCount = length * twist;
	float samplesPerTwist = (quality + 1) / 2 + 1;
	unsigned int rawRes = twistCount * samplesPerTwist;
	return MIN(MAX(rawRes, quality * 5), 100);
}

float totalTwist(vector<ancestor>& ancestors)
{
	float twist = 0.0;
	
	for (unsigned int i = ancestors.size() - 1; i < ancestors.size(); --i)
	{
		twist += ancestors[i].parent->twist;
	}

	return twist;
}

float finalRadius(vector<ancestor>& ancestors)
{
	float radius = 1.0; 

	for (unsigned int i = ancestors.size() - 1; i < ancestors.size(); --i)
	{
		radius *= (ancestors[i].parent->subs[ancestors[i].child].diameter * 0.5); 
	}

	return radius;
}

void meshBaseCasing(Geometry* geometry, vector<ancestor>& ancestors, Color color, 
	enum GeometricShape outerShape, enum GeometricShape innerShape, float length, float outerRadius, 
	float innerRadius, float twist, unsigned int quality, bool ensureVisible)
{
	// don't render casing that's extremely small
	float finalRad = finalRadius(ancestors) * outerRadius;
	if (finalRad < 0.001)
		return;

	if (ensureVisible)
		color.a = MAX(color.a, 0.1);
	// don't render casing that's extremely clear
	if (color.a < 0.001)
		return;
	
	unsigned int angularResolution = computeAngularResolution(finalRad, quality);
	unsigned int axialResolution = computeAxialResolution(length, totalTwist(ancestors) + twist, quality);
	
	uint32_t first_vert = geometry->vertices.size();
	uint32_t first_triangle = geometry->triangles.size();

	// assuming meshCylinderWall vertices end with the top row  
	unsigned int outerPointsBottomStart = geometry->vertices.size();
	meshCylinderWall(geometry, outerShape, length, outerRadius, angularResolution, axialResolution);
	unsigned int outerPointsTopStart = geometry->vertices.size() - angularResolution;

	// add vertices for bottom edge of inner shape 
	unsigned int innerPointsBottomStart = geometry->vertices.size();
	vector<Point2D> points;
	getTemplatePoints(points, angularResolution, innerShape, innerRadius);
	for (unsigned int j = 0; j < points.size(); ++j)
	{
		Point3D p;
		p.xy = points[j];
		p.z = 0.0;
		Point3D n;
		n.x = p.x;
		n.y = p.y;
		n.z = 0.0f;
		geometry->vertices.push_back(Vertex(p,n));
	}
	// add vertices for top edge of inner shape 
	unsigned int innerPointsTopStart = geometry->vertices.size();
	for (unsigned int j = 0; j < points.size(); ++j)
	{
		Point3D p;
		p.xy = points[j];
		p.z = 5.0 * length;
		Point3D n;
		n.x = p.x;
		n.y = p.y;
		n.z = 0.0f;
		geometry->vertices.push_back(Vertex(p,n));
	}
	// add triangles linking inner and outer shapes
	for (unsigned int i = 0; i < angularResolution; ++i)
	{
		uint32_t p1 = innerPointsTopStart + i;
		uint32_t p2 = innerPointsTopStart + (i+1) % angularResolution;
		uint32_t p3 = outerPointsTopStart + i;
		uint32_t p4 = outerPointsTopStart + (i+1) % angularResolution;
		geometry->triangles.push_back(Triangle(p4, p1, p3));
		geometry->triangles.push_back(Triangle(p2, p1, p4));

		p1 = innerPointsBottomStart + i;
		p2 = innerPointsBottomStart + (i+1) % angularResolution;
		p3 = outerPointsBottomStart + i;
		p4 = outerPointsBottomStart + (i+1) % angularResolution;
		geometry->triangles.push_back(Triangle(p1, p4, p3));
		geometry->triangles.push_back(Triangle(p1, p2, p4));
	}

	assert(geometry->valid());

	// Actually do the transformations on the basic canonical cylinder mesh
	bool isRotInvar = (outerShape == CIRCLE_SHAPE);
	if (!isRotInvar)
	{
		for (uint32_t v = outerPointsBottomStart; v < innerPointsBottomStart; ++v)
			applyTwistTransform(geometry->vertices[v], twist);
	}
	isRotInvar = (innerShape == CIRCLE_SHAPE);
	if (!isRotInvar)
	{
		for (uint32_t v = innerPointsBottomStart; v < geometry->vertices.size(); ++v)
			applyTwistTransform(geometry->vertices[v], twist);
	}
	isRotInvar = (outerShape == CIRCLE_SHAPE && innerShape == CIRCLE_SHAPE);
	for (uint32_t v = first_vert; v < geometry->vertices.size(); ++v)
	{
		applySubplanTransforms(geometry->vertices[v], ancestors, isRotInvar);
	}

	geometry->groups.push_back(Group(first_triangle, geometry->triangles.size() - first_triangle, 
		first_vert, geometry->vertices.size() - first_vert, color));
}

// The cane should have length between 0.0 and 1.0 and is scaled up by a factor of 5.
void meshBaseCane(Geometry* geometry, vector<ancestor>& ancestors, 
	Color color, enum GeometricShape shape, float length, float radius, float twist, 
	unsigned int quality, bool ensureVisible)
{
	// cull out geometry that's extremely small
	float finalRad = finalRadius(ancestors) * radius;
	if (finalRad < 0.01)
		return;

	// cull out geometry that's extremely clear
	if (ensureVisible)
		color.a = MAX(color.a, 0.1);
	if (color.a < 0.01)
		return;

	unsigned int angularResolution = computeAngularResolution(finalRad, quality);
	unsigned int axialResolution = computeAxialResolution(length, totalTwist(ancestors) + twist, quality);

	uint32_t first_vert = geometry->vertices.size();
	uint32_t first_triangle = geometry->triangles.size();

	meshCylinderWall(geometry, shape, length, radius, angularResolution, axialResolution);

	// now mesh top and bottom
	vector<Point2D> points;
	getTemplatePoints(points, angularResolution, shape, radius);

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
			z = 0.0;
		}

		Point3D n;
		n.x = 0.0; 
		n.y = 0.0; 
		n.z = (side ? 1.0:-1.0);

		// throw down first layer of points and triangles (central layer)
		Point3D p;
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
			geometry->triangles.push_back(Triangle(base + layer2Tris[j].c[0], base + layer2Tris[j].c[o1], 
				base + layer2Tris[j].c[o2]));
		}
	}
	assert(geometry->valid());

	// Actually do the transformations on the basic canonical cylinder mesh
	bool isRotInvar = (shape == CIRCLE_SHAPE);
	for (uint32_t v = first_vert; v < geometry->vertices.size(); ++v)
	{
		if (!isRotInvar)
			applyTwistTransform(geometry->vertices[v], twist);
		applySubplanTransforms(geometry->vertices[v], ancestors, isRotInvar);
	}

	geometry->groups.push_back(Group(first_triangle, geometry->triangles.size() - first_triangle, 
		first_vert, geometry->vertices.size() - first_vert, color));
}

void generateMesh(PickupPlan* pickup, Geometry *geometry, bool isTopLevel, unsigned int quality)
{
	if (pickup == NULL)
		return;

	geometry->clear();
	for (unsigned int i = 0; i < pickup->subs.size(); ++i)
	{
		uint32_t startVert = geometry->vertices.size();
		vector<ancestor> ancestors;
		recurseMesh(pickup->subs[i].plan, geometry, ancestors, pickup->subs[i].length, quality, isTopLevel);
		for (unsigned int j = startVert; j < geometry->vertices.size(); ++j)
		{
			applyPickupTransform(geometry->vertices[j], pickup->subs[i]); 
		}
	}
}

// recurseMesh() turns a cane into geometry that can be rendered.
// since canes can be nested, recurseMesh() processes these nestings recursively.
// keeping a running stack of ancestors nodes in the dependancy DAG
// and applying them when a leaf node is encountered.
void recurseMesh(PullPlan* plan, Geometry *geometry, vector<ancestor>& ancestors, float length, 
	unsigned int quality, bool isTopLevel)
{
	if (plan == NULL)
		return;

	// Recurse through to children 
	ancestor me = {plan, 0};
	for (unsigned int i = 0; i < plan->subs.size(); ++i)
	{
		me.child = i;
		ancestors.push_back(me);
		recurseMesh(plan->subs[i].plan, geometry, ancestors, length, quality, false);
		ancestors.pop_back();
	}

	bool ensureVisible = (ancestors.size() == 0) && isTopLevel; 
	// because we currently don't do thickness-based color,
	// we combined adjacent casing layers with the same color into
	// a single casing "interval" to avoid the "darkening" that happens 
	// by just stacking the same casing color several times 
	for (unsigned int i = 0; i < plan->getCasingCount(); ++i) 
	{
		// compute the interval of common color
		unsigned int colorIntervalStart = i;
		while (true)
		{
			if (i == plan->getCasingCount() - 1)
				break;
			// note we compare RGBA values, not GlassColor object pointers
			// because identical colors to the user likely means "equal RGBA values"
			// and not "the same library objects"
			if (plan->getCasingColor(i+1)->getColor() != plan->getCasingColor(colorIntervalStart)->getColor())
				break;
			++i;
		}

		// if you're the outermost casing in the root node of the cane,
		// you get a special flag that causes your alpha to be rounded up to some minimum
		// value to fake incidence of refraction
		bool outermostLayer = (i == plan->getCasingCount() - 1);

		if (colorIntervalStart == 0)
		{
			// punting on actually doing this geometry right and just making it a cylinder
			// (that intersects its subcanes)
			meshBaseCane(geometry, ancestors, plan->getCasingColor(colorIntervalStart)->getColor(), 
				plan->getCasingShape(i), length-0.001, plan->getCasingThickness(i), plan->twist, 
				quality, ensureVisible && outermostLayer);
		}
		else
		{
			meshBaseCasing(geometry, ancestors, plan->getCasingColor(colorIntervalStart)->getColor(), 
				plan->getCasingShape(i), plan->getCasingShape(colorIntervalStart-1), length,
				plan->getCasingThickness(i), plan->getCasingThickness(colorIntervalStart-1)+0.01, 
				plan->twist, quality, ensureVisible && outermostLayer);
		}
	}
}

}




