

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

void convert_to_mesh(Cane* c, Triangle** triangles, int* num_triangles, int illuminated_subcane, int res_mode);

#endif

