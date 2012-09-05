

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

#define TOTAL_AXIAL_RESOLUTION 6000

class Mesher
{

	public:
		Mesher();
		void generatePullMesh(PullPlan* plan, Geometry* geometry);
		void generateColorMesh(GlassColor* glassColor, Geometry* geometry);
		void generateMesh(PickupPlan* plan, Geometry* geometry);
		void generateMesh(Piece* piece, Geometry* geometry);
		void updateTotalCaneLength(Piece* piece);
		void updateTotalCaneLength(PickupPlan* plan);
		void updateTotalCaneLength(PullPlan* plan);

	private:
		// Methods
		void generateMesh(PickupPlan* plan, Geometry *geometry, bool ensureCasing, vector<PullPlan*>* ancestors, 
			vector<int>* ancestorIndices);
		void generateMesh(Piece* piece, Geometry *geometry, vector<PullPlan*>* ancestors, 
			vector<int>* ancestorIndices);
		void generateMesh(PullPlan* plan, Geometry *geometry, vector<PullPlan*>* ancestors, 
			vector<int>* ancestorIndices, float offset, float length, bool ensureVisible=false, int groupIndex = -1);

		float asymptoteVal(float s, float t);
		float splineVal(float r1, float r2, float r3, float t);
		float splineVal(float r1, float r2, float r3, float r4, float t);
		void meshPolygonalBaseCane(Geometry* geometry, vector<PullPlan*>* ancestors, vector<int>* ancestorIndices, 
			Color* color, int shape, float offset, float length, float radius, 
			bool ensureVisible, uint32_t group_tag);
		void meshPickupCasingSlab(Geometry* geometry, Color* color, float y, float thickness);
		void applyResizeTransform(Vertex* v, float scale);
		void applySubplanTransform(Vertex* v, PullPlan* parentPlan, int subplan);
		void applySubplanTransform(Geometry* geometry, PullPlan* parentPlan, int subplan);
		void applyCasingResizeTransform(Vertex* v, PullPlan* parentPlan);
		Vertex applyTransforms(Vertex p, vector<PullPlan*>* ancestors, vector<int>* ancestorIndices);
		void applyPickupTransform(Vertex* p, SubpickupTemplate* spt);
		void applyVaseTransform(Vertex* p, vector<int>* parameterValues);
		void applyTumblerTransform(Vertex* p, vector<int>* parameterValues);
		void applyBowlTransform(Vertex* p, vector<int>* parameterValues);
		void applyPotTransform(Vertex* p, vector<int>* parameterValues);
		void applyWavyPlateTransform(Vertex* p, vector<int>* parameterValues);
		float computeTotalCaneLength(Piece* piece);
		float computeTotalCaneLength(PickupPlan* plan);
		float computeTotalCaneLength(PullPlan* plan);

		// Variables
		float totalCaneLength;
};
#endif

