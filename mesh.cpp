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
#include <fstream>

//compute normals by using area-weighted normals of adjacent triangles.
//this may not be the correct approximation, but I just wanted some sort of hack
//to hold us until proper normal transforms could be written.
void Geometry::compute_normals_from_triangles() {
    assert(valid());
    std::vector< Vector3f > norms(vertices.size(), make_vector(0.0f, 0.0f, 0.0f));
    for (std::vector< Triangle >::const_iterator tri = triangles.begin(); tri != triangles.end(); ++tri) {
    Vector3f p1 = vertices[tri->v1].position;
    Vector3f p2 = vertices[tri->v2].position;
    Vector3f p3 = vertices[tri->v3].position;
    norms[tri->v1] += cross_product(p2 - p1, p3 - p1);
    norms[tri->v2] += cross_product(p3 - p2, p1 - p2);
    norms[tri->v3] += cross_product(p1 - p3, p2 - p3);
    }
    for (unsigned int v = 0; v < norms.size(); ++v) {
    if (norms[v] != make_vector(0.0f, 0.0f, 0.0f)) {
       vertices[v].normal = normalize(norms[v]);
    }
    }
}

void Geometry::save_obj_file(std::string const &filename) const {
    std::ofstream file(filename.c_str());
    for (std::vector< Vertex >::const_iterator v = vertices.begin(); v != vertices.end(); ++v) {
    file << "vn " << v->normal.x << " "
          << v->normal.y << " "
          << v->normal.z << "\n";
    file << "v " << v->position.x << " "
        << v->position.y << " "
        << v->position.z << "\n";
    }
    for (std::vector< Triangle >::const_iterator t = triangles.begin(); t != triangles.end(); ++t) {
    file << "f " << (t->v1+1) << "//" << (t->v1+1) << " "
        << (t->v2+1) << "//" << (t->v2+1) << " "
        << (t->v3+1) << "//" << (t->v3+1) << "\n";
    }
}


void Mesh::applyFlattenTransform(Vertex* v, float rectangleRatio, float rectangleTheta, float flatness)
{
    Point p_r;
    p_r.x = cos(-rectangleTheta) * v->position.x - sin(-rectangleTheta) * v->position.y;
    p_r.y = sin(-rectangleTheta) * v->position.x + cos(-rectangleTheta) * v->position.y;

    float pt_radius = length(p_r.xy);
    float pt_theta = atan2(p_r.y, p_r.x);
    if (pt_theta < 0)
    pt_theta += 2*PI;
    float arc_length = pt_radius * pt_theta;

    // We use a boundary-preserving circle to rectangle transformation
    float rect_y = PI * pt_radius / (1.0 + rectangleRatio);
    float rect_x = rect_y * rectangleRatio;
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

    v->position.x = flatness * (cos(rectangleTheta) * p_r.x
             - sin(rectangleTheta) * p_r.y) + v->position.x * (1-flatness);
    v->position.y = flatness * (sin(rectangleTheta) * p_r.x
             + cos(rectangleTheta) * p_r.y) + v->position.y * (1-flatness);
    //TODO: normal transform
}  

void Mesh :: applyBundleTransform(Vertex* v, Point location)
{
    v->position.x += location.x;
    v->position.y += location.y;
}

void Mesh :: applyStretchTransform(Vertex* v, float amount)
{
    v->position.x /= sqrt(amount);
    v->position.y /= sqrt(amount);
    v->position.z *= amount;
}

void Mesh :: applyTwistTransform(Vertex* v, float amount)
{
    float theta = atan2(v->position.y, v->position.x);
    float r = length(v->position.xy);
    theta += amount * v->position.z;
    v->position.x = r * cos(theta);
    v->position.y = r * sin(theta);
}

/*
This function applies a sequence of transformations to a 3D point.
This is used during the meshing process to determine the location of
a vertex after it has been moved via the transformations described
by its ancestors in the cane DAG. 
*/
Vertex Mesh :: applyTransforms(Vertex v, Cane** ancestors, int ancestorCount)
{
    /*
    The transformations are applied back to front to match how they
    are loaded into the array. Because the transform array is created by
    a depth-first traversal of the cane DAG, transforms lower in the DAG
    (i.e. closer to the leaves) are added later. However, they
    represent the first operations done on the cane, so need to be applied
    first.
    */
    for (int i = ancestorCount - 1; i >= 0; --i)
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
            int subcaneIndex;
            for (int j = 0; j < ancestors[i]->subcaneCount; ++j)
            {
                if (ancestors[i]->subcanes[j] == ancestors[i+1])
                {
                    subcaneIndex = j;
                    break;
                }
            }
            applyBundleTransform(&v, ancestors[i]->subcaneLocations[subcaneIndex]);
            break;
            // amts[0] describes magnitude of stretch
        case STRETCH_CANETYPE:
            applyStretchTransform(&v, ancestors[i]->amts[0]);
            //TODO: normal transform
            break;
        // amts[0] describes twist magnitude
        case TWIST_CANETYPE:
            applyTwistTransform(&v, ancestors[i]->amts[0]);
            //TODO: normal transform
            break;
        // amts[0] describes the width-to-height ratio of the goal rectangle
        // amts[1] describes the orientation w.r.t global XY
        // amts[2] describes how close of an approximation to the rectangle is achieved
        case FLATTEN_CANETYPE:
            applyFlattenTransform(&v, ancestors[i]->amts[0], ancestors[i]->amts[1],
            ancestors[i]->amts[2]);
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
    geometry->compute_normals_from_triangles();
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
            meshCircularBaseCane(geometry, ancestors, *ancestorCount, c->color, resolution);
        }
        else
        {
            Color newColor = c->color;
            newColor.a/=1;
            newColor.r/=2;
            newColor.g/=2;
            newColor.b/=2;
            meshCircularBaseCane(geometry, ancestors, *ancestorCount, newColor, resolution);
        }
    }
    else
    {
        for (i = 0; i < c->subcaneCount; ++i)
        {
            generateMesh(c->subcanes[i], geometry, ancestors, ancestorCount, resolution,isActive);
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
            updateHighResData();
        return &highResGeometry;
    }
}

void Mesh :: twistAndStretchCane(float twistAmount, float stretchAmount)
{
    if (cane == NULL)
        return;
    cane->twistAndStretch(twistAmount, stretchAmount);
    lowResDataUpToDate = 0;
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
    if (cane == NULL)
        return;
    cane->moveCane(activeSubcane, delta_x, delta_y);
    lowResDataUpToDate = 0;
    highResDataUpToDate = 0;
}

void Mesh :: addCane(Cane* c)
{
    if (cane == NULL)
        cane = c->deepCopy();
    else
        cane->add(c->deepCopy(), &activeSubcane);
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

float Mesh :: matrixDeterminant3by3(Point a,Point b,Point c) // columns
{
    float detA = a.x*(b.y*c.z-b.z*c.y)-b.x*(a.y*c.z-a.z*c.y)+c.x*(a.y*b.z-a.z*b.y);
    return detA;
}

void Mesh :: saveObjFile(std::string const &filename)
{
    updateHighResData();
    highResGeometry.save_obj_file(filename);

    updateHighResData();
    highResGeometry.save_obj_file(filename);

}

float Mesh :: intersectTriangle(Triangle plane,Point camera,Point camDir,float tCurrent=-2)
{
    Vertex va=lowResGeometry.vertices[plane.v1];
    Vertex vb=lowResGeometry.vertices[plane.v2];
    Vertex vc=lowResGeometry.vertices[plane.v3];
    Point a=va.position;
    Point b=vb.position;
    Point c=vc.position;
    Point ba=b-a;
    Point ca=c-a;

    float detA = matrixDeterminant3by3(a-b,a-c,camDir);
    float beta = matrixDeterminant3by3(a-camera,a-c,camDir)/detA;
    float gamma = matrixDeterminant3by3(a-b,a-camera,camDir)/detA;
    float t = matrixDeterminant3by3(a-camera,a-c,a-camera)/detA;

    if (beta+gamma<1 && beta>0 && gamma>0 && t>0)
    {
        if (t<tCurrent || tCurrent==-2)
            return t;
    }

    return -1;
}






