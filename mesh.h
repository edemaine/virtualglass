

#ifndef MESH_H
#define MESH_H

#include "cane.h"
#include "geometry.h"

float generate2DMesh(Cane* c, Geometry *geometry, Cane** ancestors, int* ancestorCount,
           int resolution, bool casing, bool computeRadius = false, int selectionColorIndex = -1);
float generateMesh(Cane* c, Geometry *geometry, Cane** ancestors, int* ancestorCount,
           int resolution, bool casing, bool computeRadius = false, int selectionColorIndex = -1);
float meshCircularBaseCane(Geometry *geometry,
           Cane** ancestors, int ancestorCount, int resolution, Cane *group_cane,
           uint32_t group_tag, float radius, bool computeRadius);
float mesh2DCircularBaseCane(Geometry *geometry,
           Cane** ancestors, int ancestorCount, int resolution, Cane *group_cane,
           uint32_t group_tag, float radius, bool computeRadius);
float computeTotalStretch(Cane** ancestors, int ancestorCount);
void applyFlattenTransform(Vertex* v, float rectangleRatio,
						   float rectangleTheta, float flatness);
void applyBundleTransform(Vertex* v, Point location);
void applyPullTransform(Vertex* v, float twistAmount, float stretchAmount);
Vertex applyTransforms(Vertex p, Cane** ancestors, int ancestorCount);

#endif

