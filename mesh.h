

#ifndef MESH_H
#define MESH_H

#include <vector>
#include "geometry.h"
#include "subpickuptemplate.h"
#include "piecetemplate.h"
#include "shape.h"

class GlassColor;
class PickupPlan;
class PullPlan;
class Piece;
class SubpullTemplate;
class Spline;

using std::vector;

void generateMesh(PullPlan* plan, Geometry* geometry, unsigned int quality);
void generateMesh(GlassColor* glassColor, Geometry* geometry, unsigned int quality);
void generateMesh(Piece* piece, Geometry* pieceGeometry, Geometry* pickupGeometry, unsigned int quality);

namespace MeshInternal
{
	struct ancestor
	{
		PullPlan* parent;
		unsigned int child;
	};

	// Methods
	void generateMesh(PickupPlan* plan, Geometry *geometry, bool isTopLevel, unsigned int quality);
	void recurseMesh(Piece* piece, Geometry *geometry, vector<ancestor>& ancestors, unsigned int quality);
	void recurseMesh(PullPlan* plan, Geometry *geometry, vector<ancestor>& ancestors, 
		float length, unsigned int quality, bool isTopLevel);

	float finalRadius(vector<ancestor>& ancestors);
	float totalTwist(vector<ancestor>& ancestors);
	unsigned int computeAxialResolution(float length, float twist, unsigned int quality);
	unsigned int computeAngularResolution(float diameter, unsigned int quality);
	void meshBaseCasing(Geometry* g, vector<ancestor>& ancestors, 
		Color c, enum GeometricShape outerShape, enum GeometricShape innerShape, 
		float length, float outerRadius, float innerRadius, float twist, unsigned int quality, bool ensureVisible);
	void meshBaseCane(Geometry* g, vector<ancestor>& ancestors, Color c, 
		enum GeometricShape s, float length, float radius, float twist, unsigned int quality, bool ensureVisible);
	void meshCylinderWall(Geometry* geometry, enum GeometricShape shape, float length, float radius, 
		unsigned int angularResolution, unsigned int axialResolution);
	void getTemplatePoints(vector<Vector2f>* points, unsigned int angularResolution, 
		enum GeometricShape shape, float radius);

	void applySubplanTransforms(Vertex& v, vector<ancestor>& ancestors, bool isRotInvar);
	void applySubplanTransform(Vertex& v, Point2D location);
	void applyResizeTransform(Vertex& v, float scale);
	void applyTwistTransform(Vertex& v, float twist);

	void applyPickupTransform(Vertex& p, SubpickupTemplate& spt);
	void meshPickupCasingSlab(Geometry* g, Color c, float y, float thickness);

	void casePiece(Geometry* geometry, Piece* piece);
	void applyPieceTransform(Vertex& v, float twist, Spline& spline);
	void applyPieceTransform(Geometry* geometry, Piece* piece);
}
#endif



