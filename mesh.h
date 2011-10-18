

#ifndef MESH_H
#define MESH_H

#include <vector.h>
#include "pullplan.h"
#include "geometry.h"
#include "subpulltemplate.h"

void generateMesh(PullPlan* plan, Geometry *geometry, vector<PullPlan*> ancestors, vector<int> ancestorIndices, PullPlan* casingPlan=NULL, int groupIndex = -1);
void meshPolygonalBaseCane(Geometry* geometry, vector<PullPlan*> ancestors, vector<int> ancestorIndices, PullPlan* plan, uint32_t group_tag);
void applyMoveAndResizeTransform(Vertex* v, PullPlan* parentPlan, int subplan);
void applyMoveAndResizeTransform(Geometry* geometry, PullPlan* parentPlan, int subplan);
void applyTwistTransform(Vertex* v, PullPlan* p);
void applyTwistTransform(Geometry* geometry, PullPlan* p);
Vertex applyTransforms(Vertex p, PullPlan** ancestors, int ancestorCount, bool full);


#endif

