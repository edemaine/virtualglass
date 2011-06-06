/*
The Mesh object plays the role of converting canes to 3D triangle meshes.
A cane is represented as a directed acyclic graph of nodes, each of which
represents a cane or a transformation on a cane. A 3D triangle mesh is simply
a set of triangles with vertices in 3D and specific exterior and interior
sides (we use the convention of specifying vertices in CCW order around the 
exterior face).
*/

#include "mesh.h"

// transforms should be provided top-down (and are processed bottom up)
Point Mesh :: applyTransforms(Point p, Transform* transforms, int transformCount)
{
        int i;
        float r, theta;

        for (i = transformCount-1; i >= 0; --i)
        {
                switch (transforms[i].type)
                {
                        case MOVE_TRANSFORM: // uses p_amt to describe offset from center
                                p.x += transforms[i].data.p_amt.x;
                                p.y += transforms[i].data.p_amt.y;
                                // p.z is unchanged
                                break;
                        case STRETCH_TRANSFORM: // uses f_amt to describe stretch amount
                                theta = atan2(p.y, p.x);
                                r = sqrt(p.x*p.x + p.y*p.y);
                                r /= transforms[i].data.f_amt;
                                p.x = r * cos(theta); 
                                p.y = r * sin(theta); 
                                p.z *= transforms[i].data.f_amt; // assume starting at z = 0
                                break;
                        case TWIST_TRANSFORM: // f_amt to describe twist amount per unit length 
                                theta = atan2(p.y, p.x);
                                r = sqrt(p.x*p.x + p.y*p.y);
                                theta += transforms[i].data.f_amt * p.z;
                                p.x = r * cos(theta); 
                                p.y = r * sin(theta); 
                                // p.z is unchanged
                                break;
                        case SQUAREOFF_TRANSFORM: // f_amt to describe amount of compression amount
                                p.x = p.x / transforms[i].data.f_amt;
                                p.y = p.y * transforms[i].data.f_amt;
                                // p.z is unchanged
                                break;
                        default:
                                exit(1);
                }
        }

        return p;
}


float Mesh :: computeTotalStretch(Transform* transforms, int transformCount)
{
        float totalStretch;
        int i;

        totalStretch = 1;
        for (i = 0; i < transformCount; ++i)
        {
                if (transforms[i].type == STRETCH_TRANSFORM)
                        totalStretch *= transforms[i].data.f_amt;
        }

        return totalStretch;
}

void Mesh :: meshCircularBaseCane(Triangle* triangles, int* num_triangles, Transform* transforms, int transformCount, 
        Color color, int resolution)
{
        Point p1, p2, p3, p4;
        Triangle tmp_t;
        int i, j, angularResolution, axialResolution;
        float total_stretch;

        switch (resolution)
        {
                case LOW_RESOLUTION:
                        angularResolution = LOW_ANGULAR_RESOLUTION;
                        axialResolution = LOW_AXIAL_RESOLUTION;
                        break;
                case HIGH_RESOLUTION:
                        angularResolution = HIGH_ANGULAR_RESOLUTION;
                        axialResolution = HIGH_AXIAL_RESOLUTION;
                        break;
                default:
                        exit(1);
        }

        total_stretch = computeTotalStretch(transforms, transformCount);
        
        // Get cylinder sides
        for (i = 0; i < axialResolution - 1; ++i)
        {
                for (j = 0; j < angularResolution; ++j)
                {
                        p1.x = cos(2 * PI * ((float) j) / angularResolution);
                        p1.y = sin(2 * PI * ((float) j) / angularResolution);
                        p1.z = ((float) i) / (axialResolution * total_stretch);

                        p2.x = cos(2 * PI * ((float) j) / angularResolution);
                        p2.y = sin(2 * PI * ((float) j) / angularResolution);
                        p2.z = ((float) i+1) / (axialResolution * total_stretch);

                        p3.x = cos(2 * PI * ((float) j+1) / angularResolution);
                        p3.y = sin(2 * PI * ((float) j+1) / angularResolution);
                        p3.z = ((float) i) / (axialResolution * total_stretch);

                        p4.x = cos(2 * PI * ((float) j+1) / angularResolution);
                        p4.y = sin(2 * PI * ((float) j+1) / angularResolution);
                        p4.z = ((float) i+1) / (axialResolution * total_stretch);

                        p1 = applyTransforms(p1, transforms, transformCount);
                        p2 = applyTransforms(p2, transforms, transformCount);
                        p3 = applyTransforms(p3, transforms, transformCount);
                        p4 = applyTransforms(p4, transforms, transformCount);

                        tmp_t.v1 = p2;
                        tmp_t.v2 = p1;
                        tmp_t.v3 = p4;
                        tmp_t.c = color;

                        triangles[*num_triangles] = tmp_t;
                        *num_triangles += 1;

                        tmp_t.v1 = p1;
                        tmp_t.v2 = p3;
                        tmp_t.v3 = p4;
                        tmp_t.c = color;

                        triangles[*num_triangles] = tmp_t;
                        *num_triangles += 1;
                } 
        }

        // Get cylinder bottom
        p1.x = 1.0;
        p1.y = 0.0;
        p1.z = p2.z = p3.z = 0.0;
        tmp_t.v1 = applyTransforms(p1, transforms, transformCount);
        tmp_t.c = color;

        for (j = 1; j < angularResolution-1; ++j)
        {
                p2.x = cos(2 * PI * ((float) j) / angularResolution);
                p2.y = sin(2 * PI * ((float) j) / angularResolution);
                tmp_t.v3 = applyTransforms(p2, transforms, transformCount);
                p3.x = cos(2 * PI * ((float) j+1) / angularResolution);
                p3.y = sin(2 * PI * ((float) j+1) / angularResolution);
                tmp_t.v2 = applyTransforms(p3, transforms, transformCount);
                triangles[*num_triangles] = tmp_t;
                *num_triangles += 1;
        }

        // Get cylinder top
        p1.z = p2.z = p3.z = ((float) (axialResolution-1)) / (axialResolution * total_stretch);
        tmp_t.v1 = applyTransforms(p1, transforms, transformCount);
        for (j = 1; j < angularResolution-1; ++j)
        {
                p2.x = cos(2 * PI * ((float) j) / angularResolution);
                p2.y = sin(2 * PI * ((float) j) / angularResolution);
                tmp_t.v2 = applyTransforms(p2, transforms, transformCount);
                p3.x = cos(2 * PI * ((float) j+1) / angularResolution);
                p3.y = sin(2 * PI * ((float) j+1) / angularResolution);
                tmp_t.v3 = applyTransforms(p3, transforms, transformCount);
                triangles[*num_triangles] = tmp_t;
                *num_triangles += 1;
        }
}


/*
Assumptions about parameters:

1. If c->twist or c->stretch are non-zero or non-one respective, then c has exactly one child.
2. transforms is sufficiently long for the cane.
*/
void Mesh :: generateMesh(Cane* c, Triangle* triangles, int* triangleCount, 
        Transform* transforms, int* transformCount, int resolution)
{
        int i;

        if (c == NULL)
                return;

        switch(c->type)
        {
                case TWIST_CANETYPE:
                        transforms[*transformCount].type = TWIST_TRANSFORM;
                        transforms[*transformCount].data.f_amt = c->amt;
                        *transformCount += 1;
                        generateMesh(c->subcanes[0], triangles, triangleCount, 
                                transforms, transformCount, resolution);
                        *transformCount -= 1;
                        return;
                case BASE_CIRCLE_CANETYPE: 
                        meshCircularBaseCane(triangles, triangleCount,
                                transforms, *transformCount, c->color, resolution);
                        return;
                case SQUAREOFF_CANETYPE: 
                        transforms[*transformCount].type = SQUAREOFF_TRANSFORM;
                        transforms[*transformCount].data.f_amt = c->amt;
                        *transformCount += 1;
                        generateMesh(c->subcanes[0], triangles, triangleCount, 
                                transforms, transformCount, resolution);
                        *transformCount -= 1;
                        return;
                case STRETCH_CANETYPE:
                        transforms[*transformCount].type = STRETCH_TRANSFORM;
                        transforms[*transformCount].data.f_amt = c->amt;
                        *transformCount += 1;
                        generateMesh(c->subcanes[0], triangles, triangleCount, 
                                transforms, transformCount, resolution);
                        *transformCount -= 1;
                        return;
                case BUNDLE_CANETYPE:
                        for (i = 0; i < c->subcaneCount; ++i)
                        {
                                transforms[*transformCount].type = MOVE_TRANSFORM;
                                transforms[*transformCount].data.p_amt.x = c->subcaneLocations[i].x;
                                transforms[*transformCount].data.p_amt.y = c->subcaneLocations[i].y;
                                *transformCount += 1;
                                generateMesh(c->subcanes[i], triangles, triangleCount, 
                                        transforms, transformCount, resolution);
                                *transformCount -= 1;
                        }
                        return;
                default:
                        exit(1);
        }
}

Mesh :: Mesh(Cane* c)
{
        lowResTriangles = (Triangle*) malloc(sizeof(Triangle) 
                * (LOW_AXIAL_RESOLUTION * LOW_ANGULAR_RESOLUTION * 2) * (MAX_NUM_CANES + 1));
        highResTriangles = (Triangle*) malloc(sizeof(Triangle) 
                * (HIGH_AXIAL_RESOLUTION * HIGH_ANGULAR_RESOLUTION * 2) * (MAX_NUM_CANES + 1));
        
        setCane(c);
}

void Mesh :: setCane(Cane* c)
{
        cane = c;

        updateLowResData();
        updateHighResData();
}

void Mesh :: updateLowResData()
{
        Transform transforms[MAX_TRANSFORMS];
        int transformCount;

        transformCount = 0;
        lowResTriangleCount = 0;
        generateMesh(cane, lowResTriangles, &lowResTriangleCount, transforms, &transformCount, 
                LOW_RESOLUTION);
        lowResDataUpToDate = 1;
}

void Mesh :: updateHighResData()
{
        Transform transforms[MAX_TRANSFORMS];
        int transformCount;

        transformCount = 0;
        highResTriangleCount = 0;
        generateMesh(cane, highResTriangles, &highResTriangleCount, transforms, &transformCount, 
                HIGH_RESOLUTION);
        highResDataUpToDate = 1;
}

Cane* Mesh :: getCane()
{
        return cane;
} 

Triangle* Mesh :: getMesh(int resolution)
{

        if (resolution == LOW_RESOLUTION)
        {
                if (!lowResDataUpToDate)
                        updateLowResData(); 
                return lowResTriangles;
        }
        else
        {
                if (!highResDataUpToDate)
                {
                        updateHighResData();
                        updateLowResData();
                } 
                return highResTriangles;
        }
}

int Mesh :: getNumMeshTriangles(int resolution)
{
        if (resolution == LOW_RESOLUTION)
        {
                if (!lowResDataUpToDate)
                        updateLowResData(); 
                return lowResTriangleCount;
        }
        else
        {
                if (!highResDataUpToDate)
                        updateHighResData();
                return highResTriangleCount;
        }
}

void Mesh :: twistCane(float amt)
{
        Transform twist_t;

        if (cane == NULL)
                return;
        cane->twist(amt);

        if (!lowResDataUpToDate)
                updateLowResData();

        twist_t.type = TWIST_TRANSFORM;
        twist_t.data.f_amt = amt;
        for (int i = 0; i < lowResTriangleCount; ++i)
        {
                lowResTriangles[i].v1 = applyTransforms(lowResTriangles[i].v1, &twist_t, 1);
                lowResTriangles[i].v2 = applyTransforms(lowResTriangles[i].v2, &twist_t, 1);
                lowResTriangles[i].v3 = applyTransforms(lowResTriangles[i].v3, &twist_t, 1);
        }

        lowResDataUpToDate = 1;
        highResDataUpToDate = 0;
}

void Mesh :: stretchCane(float amt)
{
        Transform stretch_t;

        if (cane == NULL)
                return;
        cane->stretch(amt);

        if (!lowResDataUpToDate)
                updateLowResData();

        stretch_t.type = STRETCH_TRANSFORM;
        stretch_t.data.f_amt = 1.0 + amt;
        for (int i = 0; i < lowResTriangleCount; ++i)
        {
                lowResTriangles[i].v1 = applyTransforms(lowResTriangles[i].v1, &stretch_t, 1);
                lowResTriangles[i].v2 = applyTransforms(lowResTriangles[i].v2, &stretch_t, 1);
                lowResTriangles[i].v3 = applyTransforms(lowResTriangles[i].v3, &stretch_t, 1);
        }

        lowResDataUpToDate = 1;
        highResDataUpToDate = 0;
}

void Mesh :: squareoffCane(float amt)
{
        Transform squareoff_t;

        if (cane == NULL)
                return;
        cane->squareoff(amt);

        if (!lowResDataUpToDate)
                updateLowResData();

        squareoff_t.type = SQUAREOFF_TRANSFORM;
        squareoff_t.data.f_amt = 1.0 + amt;
        for (int i = 0; i < lowResTriangleCount; ++i)
        {
                lowResTriangles[i].v1 = applyTransforms(lowResTriangles[i].v1, &squareoff_t, 1);
                lowResTriangles[i].v2 = applyTransforms(lowResTriangles[i].v2, &squareoff_t, 1);
                lowResTriangles[i].v3 = applyTransforms(lowResTriangles[i].v3, &squareoff_t, 1);
        }

        lowResDataUpToDate = 1;
        highResDataUpToDate = 0;
}

void Mesh :: startMoveMode()
{
        activeSubcane = 0;
        if (cane != NULL)
                cane->createBundle();
}

void Mesh :: moveCane(float delta_x, float delta_y)
{
        Transform move_t;
        int num_prev_tris, num_cur_tris;

        if (cane == NULL)
                return;

        cane->moveCane(activeSubcane, delta_x, delta_y);

        if (!lowResDataUpToDate)
                updateLowResData();

        num_prev_tris = 0;
        for (int i = 0; i < activeSubcane; ++i)
        {
                num_prev_tris += (2 * LOW_AXIAL_RESOLUTION * LOW_ANGULAR_RESOLUTION - 4) 
                        * cane->subcanes[i]->leafNodes();
        }

        num_cur_tris = (2 * LOW_AXIAL_RESOLUTION * LOW_ANGULAR_RESOLUTION - 4) 
                * cane->subcanes[activeSubcane]->leafNodes();
        move_t.type = MOVE_TRANSFORM;
        move_t.data.p_amt.x = delta_x;
        move_t.data.p_amt.y = delta_y;
        for (int i = num_prev_tris; i < num_prev_tris + num_cur_tris; ++i)
        {
                lowResTriangles[i].v1 = applyTransforms(lowResTriangles[i].v1, &move_t, 1);
                lowResTriangles[i].v2 = applyTransforms(lowResTriangles[i].v2, &move_t, 1);
                lowResTriangles[i].v3 = applyTransforms(lowResTriangles[i].v3, &move_t, 1);
        }

        lowResDataUpToDate = 1;
        highResDataUpToDate = 0;
}

void Mesh :: addCane(Cane* c)
{
        if (cane == NULL)
        {
                cane = c->deepCopy();
        }
        else
        {
                cane->add(c, &activeSubcane);
        }
        lowResDataUpToDate = highResDataUpToDate = 0;
}

void Mesh :: advanceActiveSubcane()
{
        if (cane->type != BUNDLE_CANETYPE)
                return;
        activeSubcane += 1;
        activeSubcane %= cane->subcaneCount;
        lowResDataUpToDate = highResDataUpToDate = 0;
}







