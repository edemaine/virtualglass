

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

void applyFlattenTransform(Vertex* v, Cane* transformNode);
void applyBundleTransform(Vertex* v, Cane* parentNode, int subcane);
void applyPullTransform(Vertex* v, Cane* transformNode);
void applyPullTransform(Geometry* geometry, Cane* transformNode);
Vertex applyTransforms(Vertex p, Cane** ancestors, int ancestorCount);

void unapplyFlattenTransform(Vertex* v, Cane* transformNode);
void unapplyBundleTransform(Vertex* v, Cane* parentNode, int subcane);
void unapplyPullTransform(Vertex* v, Cane* transformNode);

void unapplyFlattenTransform(Geometry* geometry, Cane* transformNode);
void unapplyBundleTransform(Geometry* geometry, Cane* parentNode, int subcane);
void unapplyPullTransform(Geometry* geometry, Cane* transformNode);

#endif

