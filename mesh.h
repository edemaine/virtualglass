

#ifndef MESH_H
#define MESH_H

#include "cane.h"
#include "geometry.h"

void generateMesh(Cane* c, Geometry *geometry, Cane** ancestors, int* ancestorCount,
	int resolution, Cane* activeSubcane, bool isActive);
void meshCircularBaseCane(Geometry *geometry,
	Cane** ancestors, int ancestorCount, Color color, int resolution);
float computeTotalStretch(Cane** ancestors, int ancestorCount);
void applyFlattenTransform(Vertex* v, float rectangleRatio,
	float rectangleTheta, float flatness);
void applyBundleTransform(Vertex* v, Point location);
void applyStretchTransform(Vertex* v, float amount);
void applyTwistTransform(Vertex* v, float amount);
Vertex applyTransforms(Vertex p, Cane** ancestors, int ancestorCount);

#endif

