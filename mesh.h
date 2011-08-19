

#ifndef MESH_H
#define MESH_H

#include "cane.h"
#include "geometry.h"

void generate2DMesh(Cane* c, Geometry *geometry, Cane** ancestors, int* ancestorCount,
					int resolution, bool fullTransforms, int selectionColorIndex = -1);
void generateMesh(Cane* c, Geometry *geometry, Cane* casingCane, Cane** ancestors, int* ancestorCount,
                                  int resolution, bool fullTransforms, bool casing = false, int selectionColorIndex = -1);
void meshCircularBaseCane(Geometry *geometry,
						  Cane** ancestors, int ancestorCount, int resolution, Cane *group_cane,
						  uint32_t group_tag, bool fullTransforms);
void meshSquareBaseCane(Geometry *geometry,
        Cane** ancestors, int ancestorCount, int resolution, Cane *group_cane,
        uint32_t group_tag, bool fullTransforms);
void mesh2DCircularBaseCane(Geometry *geometry,
							Cane** ancestors, int ancestorCount, int resolution, Cane *group_cane,
							uint32_t group_tag, bool fullTransforms);
float computeTotalStretch(Cane** ancestors, int ancestorCount);

void applyFlattenTransform(Vertex* v, Cane* transformNode);
void applyFlattenTransform(Geometry* geometry, Cane* transformNode);
void applyMoveTransform(Vertex* v, Cane* parentNode, int subcane);
void applyPartialMoveTransform(Geometry* geometry, int subcane, float deltaX, float deltaY, float deltaZ);
void applyPullTransform(Vertex* v, Cane* transformNode);
void applyPullTransform(Geometry* geometry, Cane* transformNode);
void applyCasingTransform(Vertex* v, float casingRadius);
void applyCasingTransform(Geometry* geometry, float casingRadius);
Vertex applyTransforms(Vertex p, Cane** ancestors, int ancestorCount, bool full);


#endif

