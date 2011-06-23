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
Vertex Mesh :: applyTransforms(Vertex v, Cane** ancestors, int ancestorCount)
{
        int i, j;
        float r, theta, rect_x, rect_y, pt_radius, pt_theta, arc_length;
        float* amts;
        Point p_r;

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
                                                v.position.x += ancestors[i]->subcaneLocations[j].x;
                                                v.position.y += ancestors[i]->subcaneLocations[j].y;
                                                break;
                                        }
                                }
                                // p.z is unchanged
                                break;
                        // amts[0] describes magnitude of stretch
                        case STRETCH_CANETYPE: 
                                v.position.x /= sqrt(ancestors[i]->amts[0]);
                                v.position.y /= sqrt(ancestors[i]->amts[0]);
                                v.position.z *= ancestors[i]->amts[0];
								//TODO: normal transform
                                break;
                        // amts[0] describes twist magnitude
                        case TWIST_CANETYPE: 
                                theta = atan2(v.position.y, v.position.x);
                                r = length(v.position.xy);
                                theta += ancestors[i]->amts[0] * v.position.z;
                                v.position.x = r * cos(theta); 
                                v.position.y = r * sin(theta);
								//TODO: normal transform
                                break;
                        // amts[0] describes the width-to-height ratio of the goal rectangle
                        // amts[1] describes the orientation w.r.t global XY
                        // amts[2] describes how close of an approximation to the rectangle is achieved
                        case FLATTEN_CANETYPE: 
                                amts = ancestors[i]->amts;
                                // move point to rectangle XY system
                                p_r.x = cos(-amts[1])*v.position.x - sin(-amts[1])*v.position.y; 
                                p_r.y = sin(-amts[1])*v.position.x + cos(-amts[1])*v.position.y; 
                                pt_radius = length(p_r.xy);
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
                                v.position.x = amts[2] * (cos(amts[1])*p_r.x - sin(amts[1])*p_r.y) + v.position.x * (1-amts[2]); 
                                v.position.y = amts[2] * (sin(amts[1])*p_r.x + cos(amts[1])*p_r.y) + v.position.y * (1-amts[2]); 
								//TODO: normal transform
                                break;
                        default: // BASE_CIRCLE_CANETYPE
                                break;
                }
        }

        return v;
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
void Mesh :: meshCircularBaseCane(Geometry *geometry, Cane** ancestors, 
        int ancestorCount, Color color, int resolution)
{
        unsigned int angularResolution, axialResolution;
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

        //DEBUG: total_stretch shortened... why is the top cap missing?
        total_stretch = computeTotalStretch(ancestors, ancestorCount);

        //need to know first vertex position so we can transform 'em all later
        uint32_t first_vert = geometry->vertices.size();
        
        /*
        Draw the walls of the cylinder. Note that the z location is 
        adjusted by the total stretch experienced by the cane so that
        the z values range between 0 and 1.
        */
		//Generate verts:
        for (unsigned int i = 0; i < axialResolution; ++i)
        {
                for (unsigned int j = 0; j < angularResolution; ++j)
                {
                        Point p;
                        Point n;

                        p.x = cos(2 * PI * ((float) j) / angularResolution);
                        p.y = sin(2 * PI * ((float) j) / angularResolution);
                        p.z = ((float) i) / ((axialResolution-1) * total_stretch);
                        n.x = p.x;
                        n.y = p.y;
                        n.z = 0.0f;
						geometry->vertices.push_back(Vertex(p,n,color));
				}
		}
		//Generate triangles linking them:
        for (unsigned int i = 0; i + 1 < axialResolution; ++i)
        {
                for (unsigned int j = 0; j < angularResolution; ++j)
                {
				        uint32_t p1 = first_vert + i * angularResolution + j;
				        uint32_t p2 = first_vert + (i+1) * angularResolution + j;
				        uint32_t p3 = first_vert + i * angularResolution + (j+1) % angularResolution;
				        uint32_t p4 = first_vert + (i+1) * angularResolution + (j+1) % angularResolution;
                        // Four points that define a (non-flat) quad are used
                        // to create two triangles.
                        geometry->triangles.push_back(Triangle(p2, p1, p4));
                        //was: tmp_t.v1 = p2; tmp_t.v2 = p1; tmp_t.v3 = p4;


                        geometry->triangles.push_back(Triangle(p1, p3, p4));
                        //was: tmp_t.v1 = p1; tmp_t.v2 = p3; tmp_t.v3 = p4;
                } 
        }
		assert(geometry->valid());

        /*
        Draw the cylinder bottom, then top.
        The mesh uses a set of n-2 triangles with a common vertex
        to draw a regular n-gon.
        */
        for (int side = 0; side <= 1; ++side) {
                float z = (side?1.0:0.0);
                float nz = (side?1.0:-1.0);
                uint32_t base = geometry->vertices.size();
                for (unsigned int j = 0; j < angularResolution; ++j)
                {
                        Point p;
                        p.x = cos(2 * PI * ((float) j) / angularResolution);
                        p.y = sin(2 * PI * ((float) j) / angularResolution);
                        p.z = z / total_stretch;
                        Point n;
                        n.x = 0.0; n.y = 0.0; n.z = nz;
                        geometry->vertices.push_back(Vertex(p, n, color));
                }
                if (side)
                {
                        for (unsigned int j = 1; j + 1 < angularResolution; ++j)
                        {
                                 geometry->triangles.push_back(Triangle(base, base + j, base + j + 1));
                        }
                }
                else
                {
                        for (unsigned int j = 1; j + 1 < angularResolution; ++j)
                        {
                                 geometry->triangles.push_back(Triangle(base, base + j + 1, base + j));
                        }
                }
        }
		assert(geometry->valid());

        for (uint32_t v = first_vert; v < geometry->vertices.size(); ++v)
        {
                geometry->vertices[v] = applyTransforms(geometry->vertices[v], ancestors, ancestorCount);
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
void Mesh :: generateMesh(Cane* c, Geometry *geometry,
        Cane** ancestors, int* ancestorCount, int resolution, bool isActive=false)
{
        int i;

        if (c == NULL)
                return;

        isActive = isActive || (c == getActiveSubcane());

        // Make recursive calls depending on the type of the current node
        ancestors[*ancestorCount] = c;
        *ancestorCount += 1;
        if (c->type == BASE_CIRCLE_CANETYPE)
        {
            if (isActive==false)
            {
                meshCircularBaseCane(geometry,
                        ancestors, *ancestorCount, c->color, resolution);
            }
            else
            {
                Color newColor = c->color;
                newColor.a/=1;
                newColor.r/=2;
                newColor.g/=2;
                newColor.b/=2;
                meshCircularBaseCane(geometry,
                        ancestors, *ancestorCount, newColor, resolution);
            }
        }
        else
        {
                for (i = 0; i < c->subcaneCount; ++i)
                {
                        generateMesh(c->subcanes[i], geometry,
                                ancestors, ancestorCount, resolution,isActive);
                }
        }
        *ancestorCount -= 1;
}

Mesh :: Mesh(Cane* c) : cane(NULL), lowResDataUpToDate(0), highResDataUpToDate(0), activeSubcane(0)
{
        
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
		lowResGeometry.clear();
        generateMesh(cane, &lowResGeometry, ancestors, &ancestorCount, 
                LOW_RESOLUTION);
        lowResDataUpToDate = 1;
}

void Mesh :: updateHighResData()
{
        Cane* ancestors[MAX_ANCESTORS];
        int ancestorCount;

        ancestorCount = 0;
		highResGeometry.clear();
        generateMesh(cane, &highResGeometry, ancestors, &ancestorCount, 
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
Geometry* Mesh :: getGeometry(int resolution)
{

        if (resolution == LOW_RESOLUTION)
        {
                if (!lowResDataUpToDate)
                        updateLowResData(); 
                return &lowResGeometry;
        }
        else
        {
                if (!highResDataUpToDate)
                {
                        updateHighResData();
                        updateLowResData();
                } 
                return &highResGeometry;
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

        ancestor = new Cane(TWIST_CANETYPE); //JIM sez: Memory leak
        ancestor->amts[0] = amt;
        for (size_t i = 0; i < lowResGeometry.vertices.size(); ++i)
        {
		        lowResGeometry.vertices[i] = applyTransforms(lowResGeometry.vertices[i], &ancestor, 1);
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

        ancestor = new Cane(STRETCH_CANETYPE); //JIM sez: Memory leak
        ancestor->amts[0] = (1.0 + amt);
        for (size_t i = 0; i < lowResGeometry.vertices.size(); ++i)
        {
		        lowResGeometry.vertices[i] = applyTransforms(lowResGeometry.vertices[i], &ancestor, 1);
        }

        lowResDataUpToDate = 1;
        highResDataUpToDate = 0;
}

void Mesh :: flattenCane(float rectangle_ratio, float rectangle_theta, float flatness)
{

        if (cane == NULL)
                return;

        cane->flatten(rectangle_ratio, rectangle_theta, flatness);

        lowResDataUpToDate = 0; 
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
        Cane* ancestors[2];

        if (cane == NULL)
                return;

        cane->moveCane(activeSubcane, delta_x, delta_y);

        if (!lowResDataUpToDate)
                updateLowResData();

        size_t num_prev_verts = 0;
        for (int i = 0; i < activeSubcane; ++i)
        {
                num_prev_verts += ((LOW_AXIAL_RESOLUTION + 2) * LOW_ANGULAR_RESOLUTION)
                        * cane->subcanes[i]->leafNodes();
        }

        size_t num_cur_verts = ((LOW_AXIAL_RESOLUTION + 2) * LOW_ANGULAR_RESOLUTION)
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
		//Make sure we computed some sort of reasonable numbers of verts:
		assert(num_prev_verts + num_cur_verts <= lowResGeometry.vertices.size());
        for (size_t i = num_prev_verts; i < num_prev_verts + num_cur_verts; ++i)
        {
                lowResGeometry.vertices[i] = applyTransforms(lowResGeometry.vertices[i], ancestors, 1);
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
                cane->add(c->deepCopy(), &activeSubcane);
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
        updateHighResData();
        updateLowResData();
}

Cane* Mesh :: getActiveSubcane()
{
    return getCane()->subcanes[activeSubcane];
}





