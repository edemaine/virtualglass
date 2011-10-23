

#ifndef MESH_H
#define MESH_H

#include <vector.h>
#include "pullplan.h"
#include "geometry.h"
#include "subpulltemplate.h"
#include "pickupplan.h"
#include "subpickuptemplate.h"
#include "piece.h"

void generateMesh(PullPlan* plan, Geometry *geometry, vector<PullPlan*> ancestors, vector<int> ancestorIndices, PullPlan* casingPlan=NULL, int groupIndex = -1);
void meshPolygonalBaseCane(Geometry* geometry, vector<PullPlan*> ancestors, vector<int> ancestorIndices, PullPlan* plan, uint32_t group_tag);

void generateMesh(PickupPlan* plan, Geometry *geometry, vector<PullPlan*> ancestors, vector<int> ancestorIndices);

void generateMesh(Piece* piece, Geometry *geometry, vector<PullPlan*> ancestors, vector<int> ancestorIndices);

void applyMoveAndResizeTransform(Vertex* v, PullPlan* parentPlan, int subplan);
void applyMoveAndResizeTransform(Geometry* geometry, PullPlan* parentPlan, int subplan);

void applyTwistTransform(Vertex* v, PullPlan* p);
void applyTwistTransform(Geometry* geometry, PullPlan* p);

Vertex applyTransforms(Vertex p, PullPlan** ancestors, int ancestorCount);

void applyPickupTransform(Vertex* p, SubpickupTemplate* spt);

void applyRollupTransform(Vertex* p);
void applyRollupTransform(Geometry* p);

void applySphereTransform(Vertex* p);
void applySphereTransform(Geometry* p);

#endif

