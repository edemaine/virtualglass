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
Point Mesh :: applyTransforms(Point p, Cane** ancestors, int ancestorCount)
{
        int i, j;
        float r, theta, rect_x, rect_y, pt_radius, pt_theta, arc_length;
        float* amts;
        Point interp, horiz_itsc, vert_itsc, p_r, rect_dest;
        
        /*
        The transformations are applied back to front to match how they
        are loaded into the array. Because the transform array is created by
        a depth-first traversal of the cane DAG, transforms lower in the DAG 
        (i.e. closer to the leaves) are added later. However, they 
        represent the first operations done on the cane, so need to be applied
        first. 
        */
        for (i = ancestorCount - 1; i >= 0; --i)
        {
                /*
                Each cane node has a type and an amount.
                Depending upon the type, the amount fields
                take on different meanings. For instance, a twist transform
                uses amts[0] to mean the magnitude of the twist.
                just a single real-valued parameter). 
                The BUNDLE_CANETYPE is an exception, in that it simply uses
                the location of the subcane to determine how to move the points.
                */
                switch (ancestors[i]->type)
                {
                        case BUNDLE_CANETYPE: 
                                // Find where subcane lives and apply translation 
                                // to move it to this location
                                for (j = 0; j < ancestors[i]->subcaneCount; ++j)
                                { 
                                        if (ancestors[i]->subcanes[j] == ancestors[i+1])
                                        {
                                                p.x += ancestors[i]->subcaneLocations[j].x;
                                                p.y += ancestors[i]->subcaneLocations[j].y;
                                                break;
                                        }
                                }
                                // p.z is unchanged
                                break;
                        // amts[0] describes magnitude of stretch
                        case STRETCH_CANETYPE: 
                                theta = atan2(p.y, p.x);
                                r = sqrt(p.x*p.x + p.y*p.y);
                                r /= ancestors[i]->amts[0];
                                p.x = r * cos(theta); 
                                p.y = r * sin(theta); 
                                p.z *= ancestors[i]->amts[0];
                                break;
                        // amts[0] describes twist magnitude
                        case TWIST_CANETYPE: 
                                theta = atan2(p.y, p.x);
                                r = sqrt(p.x*p.x + p.y*p.y);
                                theta += ancestors[i]->amts[0] * p.z;
                                p.x = r * cos(theta); 
                                p.y = r * sin(theta); 
                                break;
                        // amts[0] describes the width-to-height ratio of the goal rectangle
                        // amts[1] describes the orientation w.r.t global XY
                        // amts[2] describes how close of an approximation to the rectangle is achieved
                        case FLATTEN_CANETYPE: 
                                amts = ancestors[i]->amts;
                                // move point to rectangle XY system
                                p_r.x = cos(-amts[1])*p.x - sin(-amts[1])*p.y; 
                                p_r.y = sin(-amts[1])*p.x + cos(-amts[1])*p.y; 
                                pt_radius = sqrt(p_r.x*p_r.x + p_r.y*p_r.y);
                                pt_theta = atan2(p_r.y, p_r.x);
                                if (pt_theta < 0)
                                        pt_theta += 2*PI;
                                arc_length = pt_radius * pt_theta;

                                // We use a boundary-preserving circle to rectangle transformation
                                rect_y = PI * pt_radius / (1.0 + amts[0]); 
                                rect_x = rect_y * amts[0];
                                rect_y /= 2.0;
                                rect_x /= 2.0;
                                if (arc_length < rect_y)
                                {
                                        p_r.x = rect_x;
                                        p_r.y = arc_length;
                                }
                                else 
                                {
                                        arc_length -= rect_y;
                                        if (arc_length < 2 * rect_x)
                                        {
                                                p_r.x = rect_x - arc_length;
                                                p_r.y = rect_y;
                                        }
                                        else
                                        {
                                                arc_length -= 2 * rect_x;
                                                if (arc_length < 2 * rect_y)
                                                {
                                                        p_r.x = -rect_x;
                                                        p_r.y = rect_y - arc_length;
                                                }
                                                else
                                                {
                                                        arc_length -= 2 * rect_y;
                                                        if (arc_length < 2 * rect_x)
                                                        {
                                                                p_r.x = -rect_x + arc_length;
                                                                p_r.y = -rect_y;
                                                        }
                                                        else
                                                        {
                                                                arc_length -= 2 * rect_x;
                                                                p_r.x = rect_x;
                                                                p_r.y = -rect_y + arc_length;
                                                        }
                                                }
                                        }
                                }
                                p.x = amts[2] * (cos(amts[1])*p_r.x - sin(amts[1])*p_r.y) + p.x * (1-amts[2]); 
                                p.y = amts[2] * (sin(amts[1])*p_r.x + cos(amts[1])*p_r.y) + p.y * (1-amts[2]); 
                                break;
                        default: // BASE_CIRCLE_CANETYPE
                                break;
                }
        }

        return p;
}

/*
This function is used in the process of reparameterizing the mesh after 
a stretch, to a void losing vertices when the cane is cut down to just
the z-region between 0 and 1.
*/
float Mesh :: computeTotalStretch(Cane** ancestors, int ancestorCount)
{
        float totalStretch;
        int i;

        totalStretch = 1;
        for (i = 0; i < ancestorCount; ++i)
        {
                if (ancestors[i]->type == STRETCH_CANETYPE)
                        totalStretch *= ancestors[i]->amts[0];
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
void Mesh :: meshCircularBaseCane(Triangle* triangles, int* num_triangles, Cane** ancestors, 
        int ancestorCount, Color color, int resolution)
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

        total_stretch = computeTotalStretch(ancestors, ancestorCount);
        
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

                        p1 = applyTransforms(p1, ancestors, ancestorCount);
                        p2 = applyTransforms(p2, ancestors, ancestorCount);
                        p3 = applyTransforms(p3, ancestors, ancestorCount);
                        p4 = applyTransforms(p4, ancestors, ancestorCount);

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
        tmp_t.v1 = applyTransforms(p1, ancestors, ancestorCount); // Common vertex
        tmp_t.c = color;

        for (j = 1; j < angularResolution-1; ++j)
        {
                p2.x = cos(2 * PI * ((float) j) / angularResolution);
                p2.y = sin(2 * PI * ((float) j) / angularResolution);
                p3.x = cos(2 * PI * ((float) j+1) / angularResolution);
                p3.y = sin(2 * PI * ((float) j+1) / angularResolution);
                tmp_t.v3 = applyTransforms(p2, ancestors, ancestorCount);
                tmp_t.v2 = applyTransforms(p3, ancestors, ancestorCount);
                triangles[*num_triangles] = tmp_t;
                *num_triangles += 1;
        }

        p1.z = p2.z = p3.z = ((float) (axialResolution-1)) / (axialResolution * total_stretch);
        tmp_t.v1 = applyTransforms(p1, ancestors, ancestorCount);
        for (j = 1; j < angularResolution-1; ++j)
        {
                p2.x = cos(2 * PI * ((float) j) / angularResolution);
                p2.y = sin(2 * PI * ((float) j) / angularResolution);
                p3.x = cos(2 * PI * ((float) j+1) / angularResolution);
                p3.y = sin(2 * PI * ((float) j+1) / angularResolution);
                tmp_t.v2 = applyTransforms(p2, ancestors, ancestorCount);
                tmp_t.v3 = applyTransforms(p3, ancestors, ancestorCount);
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
        Cane** ancestors, int* ancestorCount, int resolution)
{
        int i;

        if (c == NULL)
                return;

        // Make recursive calls depending on the type of the current node
        ancestors[*ancestorCount] = c;
        *ancestorCount += 1;
        if (c->type == BASE_CIRCLE_CANETYPE)
        {
                meshCircularBaseCane(triangles, triangleCount,
                        ancestors, *ancestorCount, c->color, resolution);
        }
        else
        {
                for (i = 0; i < c->subcaneCount; ++i)
                {
                        generateMesh(c->subcanes[i], triangles, triangleCount, 
                                ancestors, ancestorCount, resolution);
                }
        }
        *ancestorCount -= 1;
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
        Cane* ancestors[MAX_ANCESTORS];
        int ancestorCount;

        ancestorCount = 0;
        lowResTriangleCount = 0;
        generateMesh(cane, lowResTriangles, &lowResTriangleCount, ancestors, &ancestorCount, 
                LOW_RESOLUTION);
        lowResDataUpToDate = 1;
}

void Mesh :: updateHighResData()
{
        Cane* ancestors[MAX_ANCESTORS];
        int ancestorCount;

        ancestorCount = 0;
        highResTriangleCount = 0;
        generateMesh(cane, highResTriangles, &highResTriangleCount, ancestors, &ancestorCount, 
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
        Cane* ancestor;

        if (cane == NULL)
                return;
        cane->twist(amt);

        if (!lowResDataUpToDate)
                updateLowResData();

        ancestor = new Cane(TWIST_CANETYPE);
        ancestor->amts[0] = amt;
        for (int i = 0; i < lowResTriangleCount; ++i)
        {
                lowResTriangles[i].v1 = applyTransforms(lowResTriangles[i].v1, &ancestor, 1);
                lowResTriangles[i].v2 = applyTransforms(lowResTriangles[i].v2, &ancestor, 1);
                lowResTriangles[i].v3 = applyTransforms(lowResTriangles[i].v3, &ancestor, 1);
        }

        lowResDataUpToDate = 1;
        highResDataUpToDate = 0;
}

void Mesh :: stretchCane(float amt)
{
        Cane* ancestor;

        if (cane == NULL)
                return;
        cane->stretch(amt);

        if (!lowResDataUpToDate)
                updateLowResData();

        ancestor = new Cane(STRETCH_CANETYPE);
        ancestor->amts[0] = (1.0 + amt);
        for (int i = 0; i < lowResTriangleCount; ++i)
        {
                lowResTriangles[i].v1 = applyTransforms(lowResTriangles[i].v1, &ancestor, 1);
                lowResTriangles[i].v2 = applyTransforms(lowResTriangles[i].v2, &ancestor, 1);
                lowResTriangles[i].v3 = applyTransforms(lowResTriangles[i].v3, &ancestor, 1);
        }

        lowResDataUpToDate = 1;
        highResDataUpToDate = 0;
}

void Mesh :: flattenCane(float rectangle_ratio, float rectangle_theta, float flatness)
{
        Cane* ancestor;

        if (cane == NULL)
                return;

        cane->flatten(rectangle_ratio, rectangle_theta, flatness);

        //if (!lowResDataUpToDate)
                updateLowResData();

        /*
        ancestor = new Cane(FLATTEN_CANETYPE);
        ancestor->amts[0] = (1.0 + amt);
        ancestor->amts[1] = 0.0;
        ancestor->amts[2] = 1.0;
        for (int i = 0; i < lowResTriangleCount; ++i)
        {
                lowResTriangles[i].v1 = applyTransforms(lowResTriangles[i].v1, &ancestor, 1);
                lowResTriangles[i].v2 = applyTransforms(lowResTriangles[i].v2, &ancestor, 1);
                lowResTriangles[i].v3 = applyTransforms(lowResTriangles[i].v3, &ancestor, 1);
        }

        lowResDataUpToDate = 1;
        */

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
        int num_prev_tris, num_cur_tris;
        Cane* ancestors[2];

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

        /*
        Since the subcane location is an offset from center but a moveCane() 
        call is actually a change in this offset, doing an update requires 
        simulating a movement equivalent to a small change in the offset and 
        not the offset itself. So we fake the small change on the mesh by
        throwing away the global offset and replacing it with the change.
        */
        ancestors[0] = new Cane(BUNDLE_CANETYPE);
        ancestors[1] = new Cane(UNASSIGNED_CANETYPE);
        ancestors[0]->subcaneCount = 1;
        ancestors[0]->subcanes[0] = ancestors[1];
        ancestors[0]->subcaneLocations[0].x = delta_x; 
        ancestors[0]->subcaneLocations[0].y = delta_y; 
        for (int i = num_prev_tris; i < num_prev_tris + num_cur_tris; ++i)
        {
                lowResTriangles[i].v1 = applyTransforms(lowResTriangles[i].v1, ancestors, 1);
                lowResTriangles[i].v2 = applyTransforms(lowResTriangles[i].v2, ancestors, 1);
                lowResTriangles[i].v3 = applyTransforms(lowResTriangles[i].v3, ancestors, 1);
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







