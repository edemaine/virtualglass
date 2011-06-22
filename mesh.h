

#ifndef CONVERT_H
#define CONVERT_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "cane.h"
#include "constants.h"
#include "primitives.h"

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
                void flattenCane(float rectangle_ratio, float rectangle_theta, float flatness);
                void addCane(Cane* c); 
                void advanceActiveSubcane();
                void startMoveMode();
                Cane* getActiveSubcane();

        private:
                Cane *cane;
                Triangle* lowResTriangles;
                int lowResTriangleCount;
                Triangle* highResTriangles;
                int highResTriangleCount;
                int lowResDataUpToDate;
                int highResDataUpToDate;
                int activeSubcane;
                Point* activePoints;
        
                void generateMesh(Cane* c, Triangle* triangles, int* triangleCount,
                        Cane** ancestors, int* ancestorCount, int resolution,bool isActive);
                void meshCircularBaseCane(Triangle* triangles, int* triangleCount, 
                        Cane** ancestors, int ancestorCount, Color color, int resolution);
                float computeTotalStretch(Cane** ancestors, int ancestorCount);
                Point applyTransforms(Point p, Cane** ancestors, int ancestorCount);
                void updateLowResData();
                void updateHighResData();
};

#endif

