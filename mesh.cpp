/*
The Mesh object plays the role of converting canes to 3D triangle meshes.
A cane is represented as a directed acyclic graph of nodes, each of which
represents a cane or a transformation on a cane. A 3D triangle mesh is simply
a set of triangles with vertices in 3D and specific exterior and interior
sides (we use the convention of specifying vertices in CCW order around the 
exterior face).

A Mesh object holds both a cane, and the Triangle arrays corresponding to a
3D mesh of the cane. If the cane changes, it must be changed through one
of the Mesh functions, and the Mesh object has functions that match all of the
Cane functions (twist, stretch, etc.).

Of course, the Mesh object can also have the Cane object which it meshes updated,
for instance, if a new cane is loaded. However, this should be done if the
cane being operated on is truly a new cane, and not just a modified version
of the current cane. The reason is efficiency. Making calls to Mesh.twistCane()
allows the Mesh object to quickly update the Triangle array (via direct operations
on the array) instead of recomputing a mesh by traversing the cane.
*/

#include "mesh.h"

/*
This function applies a sequence of transformations to a 3D point.
This is used during the meshing process to determine the location of
a vertex after it has been moved via the transformations described
by its ancestors in the cane DAG. 
*/
Point Mesh :: applyTransforms(Point p, Transform* transforms, int transformCount)
{
        int i;
        float r, theta;

        /*
        The transformations are applied back to front to match how they
        are loaded into the array. Because the transform array is created by
        a depth-first traversal of the cane DAG, transforms lower in the DAG 
        (i.e. closer to the leaves) are added later. However, they 
        represent the first operations done on the cane, so need to be applied
        first. 
        */
        for (i = transformCount-1; i >= 0; --i)
        {
                /*
                Each transformation has a type and an amount.
                Depending upon the type of transformation, the amount fields
                take on different meanings. For instance, a twist transform
                uses the `f_amt' or `float amount' field (since a twist has 
                just a single real-valued parameter). The move transform has
                two parameters, so it uses the `p_amt' or `point amount' field,
                though the movement is really a vector, but whatever.
                */
                switch (transforms[i].type)
                {
                        // p_amt field describes offset from the z-axis
                        case MOVE_TRANSFORM: 
                                p.x += transforms[i].data.p_amt.x;
                                p.y += transforms[i].data.p_amt.y;
                                // p.z is unchanged
                                break;
                        // f_amt field describes magnitude of stretch
                        case STRETCH_TRANSFORM: 
                                theta = atan2(p.y, p.x);
                                r = sqrt(p.x*p.x + p.y*p.y);
                                r /= transforms[i].data.f_amt;
                                p.x = r * cos(theta); 
                                p.y = r * sin(theta); 
                                p.z *= transforms[i].data.f_amt; // assume starting at z = 0
                                break;
                        // f_amt describes twist magnitude
                        case TWIST_TRANSFORM: 
                                theta = atan2(p.y, p.x);
                                r = sqrt(p.x*p.x + p.y*p.y);
                                theta += transforms[i].data.f_amt * p.z;
                                p.x = r * cos(theta); 
                                p.y = r * sin(theta); 
                                break;
                        // f_amt describes magnitude of deformation 
                        case FLATTEN_TRANSFORM: 
                                p.x = p.x / transforms[i].data.f_amt;
                                p.y = p.y * transforms[i].data.f_amt;
                                break;
                        default:
                                exit(1);
                }
        }

        return p;
}

/*
This function is used in the process of reparameterizing the mesh after 
a stretch, to a void losing vertices when the cane is cut down to just
the z-region between 0 and 1.
*/
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

/*
Mesh::meshCircularBaseCane() creates a mesh for a radius 1, length 1 cylindrical piece of cane,
and applies a sequences of transforms (coming from a depth-first traversal of the cane ending
with this leaf base cane). The triangles are added to the end of the array passed in. 

The resolution refers to the dual resolution modes used by the GUI, and the actual number of
triangles for these resolutions are set in constants.h 
*/
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
        
        /*
        Draw the walls of the cylinder. Note that the z location is 
        adjusted by the total stretch experienced by the cane so that
        the z values range between 0 and 1.
        */
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

                        // Four points that define a (non-flat) quad are used
                        // to create two triangles.
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

        /*
        Draw the cylinder bottom, then top.
        The mesh uses a set of n-2 triangles with a common vertex
        to draw a regular n-gon.
        */
        p1.x = 1.0;
        p1.y = 0.0;
        p1.z = p2.z = p3.z = 0.0;
        tmp_t.v1 = applyTransforms(p1, transforms, transformCount); // Common vertex
        tmp_t.c = color;

        for (j = 1; j < angularResolution-1; ++j)
        {
                p2.x = cos(2 * PI * ((float) j) / angularResolution);
                p2.y = sin(2 * PI * ((float) j) / angularResolution);
                p3.x = cos(2 * PI * ((float) j+1) / angularResolution);
                p3.y = sin(2 * PI * ((float) j+1) / angularResolution);
                tmp_t.v3 = applyTransforms(p2, transforms, transformCount);
                tmp_t.v2 = applyTransforms(p3, transforms, transformCount);
                triangles[*num_triangles] = tmp_t;
                *num_triangles += 1;
        }

        p1.z = p2.z = p3.z = ((float) (axialResolution-1)) / (axialResolution * total_stretch);
        tmp_t.v1 = applyTransforms(p1, transforms, transformCount);
        for (j = 1; j < angularResolution-1; ++j)
        {
                p2.x = cos(2 * PI * ((float) j) / angularResolution);
                p2.y = sin(2 * PI * ((float) j) / angularResolution);
                p3.x = cos(2 * PI * ((float) j+1) / angularResolution);
                p3.y = sin(2 * PI * ((float) j+1) / angularResolution);
                tmp_t.v2 = applyTransforms(p2, transforms, transformCount);
                tmp_t.v3 = applyTransforms(p3, transforms, transformCount);
                triangles[*num_triangles] = tmp_t;
                *num_triangles += 1;
        }
}


/*
Mesh::generateMesh() is the top-level function for turning a cane into an
array of triangles. The triangle and transform arrays passed in are meant
to be reusable, global arrays that are allocated in advance are sufficiently 
large. As generateMesh() is called recursively, the transforms array is
filled with with the transformations encountered at each node. When a 
leaf is reached, these transformations are used to generate a complete mesh
for the leaf node.
*/
void Mesh :: generateMesh(Cane* c, Triangle* triangles, int* triangleCount, 
        Transform* transforms, int* transformCount, int resolution)
{
        int i;

        if (c == NULL)
                return;

        // Make recursive calls depending on the type of the current node
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
                case FLATTEN_CANETYPE: 
                        transforms[*transformCount].type = FLATTEN_TRANSFORM;
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
        // Allocate space for the two permanent arrays that hold both mesh versions 
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

/*
Mesh::updateLowResData() and similarly updateHighResData() update the two
Triangle arrays containing the two versions of the mesh. They are called
when the mesh becomes out of date and is requested (by OpenGLWidget, for instance).
*/
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

/*
Mesh::getMesh() is called by an external entity that
wants to get an up-to-date mesh of the cane currently
held by this Mesh object.

If the mesh is out-of-date, it is updated, otherwise
the current mesh is simply returned.

Mesh::getNumMeshTriangles() is a companion method that
returns the size of the array whose pointer is returned
by getMesh().
*/
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

/*
The Mesh :: *Cane() methods (e.g. Mesh.twistCane(), Mesh.stretchCane(),..)
are used to modify the cane held by the Mesh object in small ways. They
should be used whenever possible over Mesh.setCane() because they are way
more efficient. The technique is essentially to update the cane in the 
background, and directly apply the twist transformation on the mesh vertices
if possible instead of rebuilding the mesh from scratch from the cane object.
*/
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

void Mesh :: flattenCane(float amt)
{
        Transform flatten_t;

        if (cane == NULL)
                return;
        cane->flatten(amt);

        if (!lowResDataUpToDate)
                updateLowResData();

        flatten_t.type = FLATTEN_TRANSFORM;
        flatten_t.data.f_amt = 1.0 + amt;
        for (int i = 0; i < lowResTriangleCount; ++i)
        {
                lowResTriangles[i].v1 = applyTransforms(lowResTriangles[i].v1, &flatten_t, 1);
                lowResTriangles[i].v2 = applyTransforms(lowResTriangles[i].v2, &flatten_t, 1);
                lowResTriangles[i].v3 = applyTransforms(lowResTriangles[i].v3, &flatten_t, 1);
        }

        lowResDataUpToDate = 1;
        highResDataUpToDate = 0;
}

/*
Right now Mesh::startMoveMode() is a placeholder mostly.
However, when subcane illumination (used to indicate to
the user which subcane is currently selected) is reimplemented,
it will perform the change in mesh colors needed to do
the indication. 
*/
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

/*
Mesh::advanceActiveSubcane() is used when in bundle
mode (i.e. the root node of the cane being operated on
is a bundle) to change the subcane currently under user
control. It will (i.e. in the future) 
also change the colors of the subcanes
to illuminate the newly selected subcane.
*/
void Mesh :: advanceActiveSubcane()
{
        if (cane->type != BUNDLE_CANETYPE)
                return;
        activeSubcane += 1;
        activeSubcane %= cane->subcaneCount;
        lowResDataUpToDate = highResDataUpToDate = 0;
}







