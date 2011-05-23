

#ifndef CONVERT_H
#define CONVERT_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "cane.h"
#include "constants.h"
#include "primitives.h"

// Object modelling stuff
typedef struct TransformData
{
        float f_amt;
        Point p_amt;
} TransformData;

typedef struct Transform
{
        int type;
        TransformData data;
} Transform;

// Mesh stuff
typedef struct Triangle
{
        Point v1;
        Point v2;
        Point v3;
        Color c;
} Triangle;

class Mesh
{

        public:
                Mesh(Cane* c);
                void setCane(Cane* c);
                Cane* getCane();
                Triangle* getMesh(int resolution);
                int getNumMeshTriangles(int resolution);
                void twistCane(float amt);
                void stretchCane(float amt);
                void moveCane(float delta_x, float delta_y);
                void squareoffCane(float amt);
                void addCane(Cane* c); 
                void advanceActiveSubcane();
                void startMoveMode();

        private:
                Cane *cane;
                Triangle* lowResTriangles;
                int lowResTriangleCount;
                Triangle* highResTriangles;
                int highResTriangleCount;
                int lowResDataUpToDate;
                int highResDataUpToDate;
                int activeSubcane;
        
                void generateMesh(Cane* c, Triangle* triangles, int* triangleCount,
                        Transform* transforms, int* transformCount, int resolution);
                void meshCircularBaseCane(Triangle* triangles, int* triangleCount, 
                        Transform* transforms, int transformCount, Color color, int resolution);
                float computeTotalStretch(Transform* transforms, int transformCount);
                Point applyTransforms(Point p, Transform* transforms, int transformCount);
                void updateLowResData();
                void updateHighResData();
};

#endif

