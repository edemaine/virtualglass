

#ifndef MESH_H
#define MESH_H

#include <vector>
#include "pullplan.h"
#include "geometry.h"
#include "subpulltemplate.h"
#include "pickupplan.h"
#include "subpickuptemplate.h"
#include "piece.h"
#include "glasscolor.h"
#include "shape.h"

class Mesher
{

	public:
		Mesher();
		void generatePullMesh(PullPlan* plan, Geometry* geometry);
		void generateColorMesh(GlassColor* glassColor, Geometry* geometry);
		void generateMesh(PickupPlan* plan, Geometry* geometry);
		void generateMesh(Piece* piece, Geometry* geometry);

	private:

		struct ancestor
		{
			PullPlan* parent;
			unsigned int child;
		};

		// Methods
		void recurseMesh(PickupPlan* plan, Geometry *geometry, vector<ancestor>* ancestors);
		void recurseMesh(Piece* piece, Geometry *geometry, vector<ancestor>* ancestors);
		void recurseMesh(PullPlan* plan, Geometry *geometry, vector<ancestor>* ancestors,
			float length, int groupIndex = -1);

		float asymptoteVal(float s, float t);
		float splineVal(float r1, float r2, float r3, float t);
		float splineVal(float r1, float r2, float r3, float r4, float t);
		void meshBaseCasing(Geometry* g, vector<ancestor>* ancestors, 
			Color* c, enum GeometricShape outerShape, enum GeometricShape innerShape, 
			float length, float outerRadius, float innerRadius, uint32_t group_tag, bool ensureVisible=false);
		void meshBaseCane(Geometry* g, vector<ancestor>* ancestors, Color* c, 
			enum GeometricShape s, float length, float radius, uint32_t group_tag);
		void meshCylinderWall(Geometry* geometry, enum GeometricShape shape, float length, float radius, 
			unsigned int angularResolution, unsigned int axialResolution, bool flipOrientation=false);
		void getTemplatePoints(vector<Vector2f>* points, unsigned int angularResolution, 
			enum GeometricShape shape, float radius);
		void meshPickupCasingSlab(Geometry* g, Color* c, float y, float thickness);
		void applyResizeTransform(Vertex* v, float scale);
		void applySubplanTransform(Vertex* v, ancestor a);
		void applySubplanTransform(Geometry* geometry, ancestor a);
		void applyCasingResizeTransform(Vertex* v, ancestor a);
		Vertex applyTransforms(Vertex p, vector<ancestor>* ancestors);
		void applyPickupTransform(Vertex* p, SubpickupTemplate* spt);
		void applyPieceTransform(Geometry* geometry, enum PieceTemplate::Type type, vector<TemplateParameter> params);
		float totalShrink(vector<ancestor>* ancestors);
};
#endif



