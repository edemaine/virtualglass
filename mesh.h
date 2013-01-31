

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

class Mesher
{

	public:
		Mesher();
		void generatePullMesh(PullPlan* plan, Geometry* geometry);
		void generateColorMesh(GlassColor* glassColor, Geometry* geometry);
		void generateMesh(PickupPlan* plan, Geometry* geometry, bool viewAll = false);
		void	generateMesh(Piece* piece, Geometry* geometry, bool);

	private:

		struct ancestor
		{
			PullPlan* parent;
			unsigned int child;
		};

		// Methods
		void recurseMesh(PickupPlan* plan, Geometry *geometry, vector<ancestor>* ancestors, bool isTopLevel=false, bool viewAll = false, float thickness = 0, float slabShift = 0);
		void recurseMesh(Piece* piece, Geometry *geometry, vector<ancestor>* ancestors, bool viewAll = false);
		void recurseMesh(PullPlan* plan, Geometry *geometry, vector<ancestor>* ancestors, 
			float length, bool isTopLevel=false);

		float asymptoteVal(float s, float t);
		float splineVal(float r1, float r2, float r3, float t);
		float splineVal(float r1, float r2, float r3, float r4, float t);
		void meshBaseCasing(Geometry* g, vector<ancestor>* ancestors, 
			Color* c, enum GeometricShape outerShape, enum GeometricShape innerShape, 
			float length, float outerRadius, float innerRadius, bool ensureVisible=false);
		void meshBaseCane(Geometry* g, vector<ancestor>* ancestors, Color* c, 
			enum GeometricShape s, float length, float radius);
		void meshCylinderWall(Geometry* geometry, enum GeometricShape shape, float length, float radius, 
			unsigned int angularResolution, unsigned int axialResolution, bool flipOrientation=false);
		void getTemplatePoints(vector<Vector2f>* points, unsigned int angularResolution, 
			enum GeometricShape shape, float radius);
		void meshPickupCasingSlab(Geometry* g, Color* c, float y, float thickness, float slabShift = 0, bool ensureVisible=false);
		void applyResizeTransform(Vertex* v, float scale);
		void applySubplanTransform(Vertex* v, ancestor a);
		void applySubplanTransform(Geometry* geometry, ancestor a);
		void applyCasingResizeTransform(Vertex* v, ancestor a);
		Vertex applyTransforms(Vertex p, vector<ancestor>* ancestors);
		void applyPickupTransform(Vertex* p, SubpickupTemplate* spt);
		void applyPieceTransform(Geometry* geometry, enum PieceTemplate::Type type, vector<TemplateParameter> params, vector <int>* vecLayerTwist, vector<unsigned int>* vecLayerVertices = new vector <unsigned int>());
		float totalShrink(vector<ancestor>* ancestors);
};
#endif



