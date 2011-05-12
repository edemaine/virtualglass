

#ifndef TYPES_H
#define TYPES_H

#include "constants.h"

typedef struct PolarPoint
{
        float r;
        float theta;
} PolarPoint;

typedef struct Point
{
    float x;
    float y;
    float z;
} Point;

typedef struct Color
{
    float r;
    float g;
    float b;
    float trans;
} Color;

typedef struct Triangle
{
    Point v1;
    Point v2;
    Point v3;
    Color c;
} Triangle;


struct Cane
{
        float stretch;
        float twist;
        int num_subcanes;
        PolarPoint subcane_locs[MAX_SUBCANE_COUNT];
        struct Cane* subcanes[MAX_SUBCANE_COUNT];
        Color color;
};

typedef struct Cane Cane; 


#endif
 
