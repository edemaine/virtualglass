

#ifndef MESH_H
#define MESH_H

#include <vector>
#include "pullplan.h"
#include "geometry.h"
#include "subpulltemplate.h"
#include "pickupplan.h"
#include "subpickuptemplate.h"
#include "piece.h"

class Mesher
{

	public:
		Mesher();
		void generateMesh(PickupPlan* plan, Geometry *geometry, vector<PullPlan*>* ancestors, 
			vector<int>* ancestorIndices);
		void generateMesh(Piece* piece, Geometry *geometry, vector<PullPlan*>* ancestors, 
			vector<int>* ancestorIndices);
		void generateMesh(PullPlan* plan, Geometry *geometry, vector<PullPlan*>* ancestors, 
			vector<int>* ancestorIndices, float start, float end, bool addCasing = false, int groupIndex = -1);
		void updateTotalCaneLength(Piece* piece);
		void updateTotalCaneLength(PickupPlan* plan);
		void updateTotalCaneLength(PullPlan* plan);

	private:
		// Methods
		float splineVal(float r1, float r2, float r3, float t);
		float splineVal(float r1, float r2, float r3, float r4, float t);
		float tableCos(float theta);
		float tableSin(float theta);
		void meshPolygonalBaseCane(Geometry* geometry, vector<PullPlan*>* ancestors, vector<int>* ancestorIndices, 
			PullPlan* plan, float start, float end, uint32_t group_tag);
		void applyMoveAndResizeTransform(Vertex* v, PullPlan* parentPlan, int subplan);
		void applyMoveAndResizeTransform(Geometry* geometry, PullPlan* parentPlan, int subplan);
		void applyTwistTransform(Vertex* v, PullPlan* p);
		void applyTwistTransform(Geometry* geometry, PullPlan* p);
		Vertex applyTransforms(Vertex p, vector<PullPlan*>* ancestors, vector<int>* ancestorIndices);
		void applyPickupTransform(Vertex* p, SubpickupTemplate* spt);
		void applyVaseTransform(Vertex* p, vector<int>* parameterValues);
		void applyTumblerTransform(Vertex* p, vector<int>* parameterValues);
		void applyBowlTransform(Vertex* p, vector<int>* parameterValues);
		float computeTotalCaneLength(Piece* piece);
		float computeTotalCaneLength(PickupPlan* plan);
		float computeTotalCaneLength(PullPlan* plan);

		// Variables
		int trigTableSize;
		vector<float> cosTable;
		vector<float> sinTable;
		float totalCaneLength;
		PullPlan* circleCasing;
		PullPlan* squareCasing;
};
#endif

