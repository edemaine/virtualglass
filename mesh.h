

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
		// Methods
		void generateMesh(PickupPlan* plan, Geometry *geometry, vector<PullPlan*>* ancestors, 
			vector<int>* ancestorIndices);
		void generateMesh(Piece* piece, Geometry *geometry, vector<PullPlan*>* ancestors, 
			vector<int>* ancestorIndices);
		void generateMesh(PullPlan* plan, Geometry *geometry, vector<PullPlan*>* ancestors, 
			vector<int>* ancestorIndices, float length, int groupIndex = -1);

		float asymptoteVal(float s, float t);
		float splineVal(float r1, float r2, float r3, float t);
		float splineVal(float r1, float r2, float r3, float r4, float t);
		void meshBaseCasing(Geometry* g, vector<PullPlan*>* ancestors, vector<int>* ancestorIndices, 
			Color* c, enum GeometricShape outerShape, enum GeometricShape innerShape, 
			float length, float outerRadius, float innerRadius, uint32_t group_tag);
		void meshBaseCane(Geometry* g, vector<PullPlan*>* ancestors, vector<int>* ancestorIndices, 
			Color* c, enum GeometricShape s, float length, float radius, 
			uint32_t group_tag);
		void meshCylinderWall(Geometry* geometry, enum GeometricShape shape, float length, float radius, 
			unsigned int angularResolution, unsigned int axialResolution, bool flipOrientation=false);
		void getTemplatePoints(vector<Vector2f>* points, unsigned int angularResolution, enum GeometricShape shape, float radius);
		void meshPickupCasingSlab(Geometry* g, Color* c, float y, float thickness);
		void applyResizeTransform(Vertex* v, float scale);
		void applySubplanTransform(Vertex* v, PullPlan* parentPlan, int subplan);
		void applySubplanTransform(Geometry* geometry, PullPlan* parentPlan, int subplan);
		void applyCasingResizeTransform(Vertex* v, PullPlan* parentPlan);
		Vertex applyTransforms(Vertex p, vector<PullPlan*>* ancestors, vector<int>* ancestorIndices);
		void applyPickupTransform(Vertex* p, SubpickupTemplate* spt);
		void applyPieceTransform(Geometry* geometry, enum PieceTemplate::Type type, vector<TemplateParameter> params);
		float totalShrink(vector<PullPlan*>* ancestors, vector<int>* ancestorIndices);
};
#endif

