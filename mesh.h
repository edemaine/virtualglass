

#ifndef MESH_H
#define MESH_H

#include <QObject>
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

using std::vector;

void generatePullMesh(PullPlan* plan, Geometry* geometry, unsigned int quality);
void generateColorMesh(GlassColor* glassColor, Geometry* geometry, unsigned int quality);
void generateMesh(PickupPlan* plan, Geometry* geometry, unsigned int quality);
void generateMesh(Piece* piece, Geometry* geometry, unsigned int quality);

namespace MeshInternal
{
	struct ancestor
	{
		PullPlan* parent;
		unsigned int child;
	};

	// Methods
	void recurseMesh(PickupPlan* plan, Geometry *geometry, vector<ancestor>* ancestors, unsigned int quality,
		bool isTopLevel=false);
	void recurseMesh(Piece* piece, Geometry *geometry, vector<ancestor>* ancestors, unsigned int quality);
	void recurseMesh(PullPlan* plan, Geometry *geometry, vector<ancestor>* ancestors, 
		float length, unsigned int quality, bool isTopLevel=false);

	float asymptoteVal(float s, float t);
	float splineVal(float r1, float r2, float r3, float t);
	float splineVal(float r1, float r2, float r3, float r4, float t);
	void meshBaseCasing(Geometry* g, vector<ancestor>* ancestors, 
		Color c, enum GeometricShape outerShape, enum GeometricShape innerShape, 
		float length, float outerRadius, float innerRadius, unsigned int quality, bool ensureVisible=false);
	void meshBaseCane(Geometry* g, vector<ancestor>* ancestors, Color c, 
		enum GeometricShape s, float length, float radius, unsigned int quality, bool ensureVisible);
	void meshCylinderWall(Geometry* geometry, enum GeometricShape shape, float length, float radius, 
		unsigned int angularResolution, unsigned int axialResolution, bool flipOrientation=false);
	void getTemplatePoints(vector<Vector2f>* points, unsigned int angularResolution, 
		enum GeometricShape shape, float radius);
	void meshPickupCasingSlab(Geometry* g, Color c, float y, float thickness, bool ensureVisible=false);
	void applyResizeTransform(Vertex* v, float scale);
	void applySubplanTransform(Vertex* v, ancestor a);
	void applySubplanTransform(Geometry* geometry, ancestor a);
	void applyCasingResizeTransform(Vertex* v, ancestor a);
	Vertex applyTransforms(Vertex p, vector<ancestor>* ancestors);
	void applyPickupTransform(Vertex* p, SubpickupTemplate* spt);
	void applyPieceTransform(Geometry* geometry, enum PieceTemplate::Type type, vector<TemplateParameter> params);
	float totalShrink(vector<ancestor>* ancestors);
}
#endif



