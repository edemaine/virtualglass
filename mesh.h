

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
                void stretchCane(float amt, float max_stretch);
                void moveCane(int curActiveSubcane, float delta_x, float delta_y);
                void squareoffCane(float amt, float max_squareoff);
                void addCane(Cane* c, int* active_subcane); 
                void advanceActiveSubcane(int* active_subcane);
                void setIlluminatedSubcane(int new_ill_subcane);

        private:
                Cane *cane;
                Triangle* low_res_tris;
                int num_low_res_tris;
                Triangle* high_res_tris;
                int num_high_res_tris;
                int illuminated_subcane;
        
                float generateMesh(Cane* c, Triangle* triangles, int* num_triangles,
                        Transform* Ts, int* num_Ts, int illuminated_subcane, int global_wrap, int res_mode);
                float convert_circular_cane_to_addl_triangles(Triangle* triangles, 
                        int* num_triangles, Transform* Ts, int num_Ts, Color color, 
                        int illuminated_subcane, int res_mode);
                float compute_total_stretch(Transform* Ts, int num_Ts);
                Point apply_transforms(Point p, Transform* Ts, int num_Ts);

};

#endif

