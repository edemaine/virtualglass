

#ifndef MESH_H
#define MESH_H

#include <vector>
#include <time.h>

#include "geometry.h"
#include "subpickuptemplate.h"
#include "piecetemplate.h"
#include "shape.h"

class GlassColor;
class Pickup;
class Cane;
class Piece;
class SubcaneTemplate;
class Spline;

using std::vector;

void generateMesh(GlassColor* glassColor, Geometry* geometry, unsigned int quality);
bool generateMesh(Cane* cane, Geometry* geometry, unsigned int quality);
bool generateMesh(Piece* piece, Geometry* pieceGeometry, Geometry* pickupGeometry, unsigned int quality);

namespace MeshInternal
{
	struct Ancestor
	{
		Cane* parent;
		unsigned int child;
	};

	struct CasingData
	{
		Color color;
		enum GeometricShape outerShape;
		float outerRadius;
		enum GeometricShape innerShape;
		float innerRadius;
		float length;
		float twist;
	};

	struct CaneData
	{
		Color color;	
		enum GeometricShape shape;
		float radius;
		float length;
		float twist;
	};

	// Methods
	void generatePickupMesh(Piece* piece, Geometry *geometry, bool isTopLevel, unsigned int quality, clock_t end);
	void generateMesh(Cane* cane, Geometry* geometry, unsigned int quality, clock_t end);
	void recurseMesh(Cane* cane, Geometry *geometry, vector<Ancestor>& ancestors, 
		float length, unsigned int quality, bool isTopLevel, clock_t end);

	float finalRadius(vector<Ancestor>& ancestors);
	float totalTwist(vector<Ancestor>& ancestors);
	unsigned int computeAxialResolution(float length, float twist, unsigned int quality);
	unsigned int computeAngularResolution(float diameter, unsigned int quality, enum GeometricShape shape);
	void meshBaseCasing(Geometry* g, vector<Ancestor>& ancestors, struct CasingData casing, 
		unsigned int quality, bool ensureVisible);
	void meshBaseCane(Geometry* g, vector<Ancestor>& ancestors, struct CaneData cane, 
		unsigned int quality, bool ensureVisible);
	void meshCylinderWall(Geometry* geometry, enum GeometricShape shape, float length, float radius, 
		unsigned int angularResolution, unsigned int axialResolution);
	void getTemplatePoints(vector<Point2D>& points, unsigned int angularResolution, 
		enum GeometricShape shape, float radius);

	void applySubcaneTransform(Vertex& v, Point2D location);
	void applyResizeTransform(Vertex& v, float scale);
	void applyTwistTransform(Vertex& v, float twist);
	void applyCaneTransform(Vertex& v, Ancestor a);

	void applyPickupTransform(Vertex& p, SubpickupTemplate& spt);
	void meshPickupCasingSlab(Geometry* g, Color c, float y, float thickness);

	void casePiece(Geometry* geometry, Piece* piece);
	void applyPieceTransform(Vertex& v, float twist, Spline& spline);
	void applyPieceTransform(Geometry* geometry, Piece* piece);
}
#endif



