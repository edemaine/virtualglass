/*
Converts a cane (constructed as a DAG) into a set of triangles
in 3-space, each with a set of coordinates and a color. 
*/

#include "mesh.h"

Color brighten_color(Color c)
{
        Color i;
        i.r = MIN(c.r + 0, 1.0); 
        i.g = MIN(c.g + 0, 1.0); 
        i.b = MIN(c.b + 0, 1.0); 
        i.a = c.a;
        return i;
}

Color darken_color(Color c)
{
        Color i;
        i.r = MAX(c.r - 0.1, 0.0); 
        i.g = MAX(c.g - 0.1, 0.0); 
        i.b = MAX(c.b - 0.1, 0.0); 
        i.a = c.a;
        return i;
}

// transforms should be provided top-down (and are processed bottom up)
Point Mesh :: apply_transforms(Point p, Transform* Ts, int num_Ts)
{
        int i;
        float r, theta;

        for (i = num_Ts-1; i >= 0; --i)
        {
                switch (Ts[i].type)
                {
                        case MOVE_TRANSFORM: // uses p_amt to describe offset from center
                                p.x += Ts[i].data.p_amt.x;
                                p.y += Ts[i].data.p_amt.y;
                                // p.z is unchanged
                                break;
                        case STRETCH_TRANSFORM: // uses f_amt to describe stretch amount
                                theta = atan2(p.y, p.x);
                                r = sqrt(p.x*p.x + p.y*p.y);
                                r /= Ts[i].data.f_amt;
                                p.x = r * cos(theta); 
                                p.y = r * sin(theta); 
                                p.z *= Ts[i].data.f_amt; // assume starting at z = 0
                                break;
                        case TWIST_TRANSFORM: // f_amt to describe twist amount per unit length 
                                theta = atan2(p.y, p.x);
                                r = sqrt(p.x*p.x + p.y*p.y);
                                theta += Ts[i].data.f_amt * p.z;
                                p.x = r * cos(theta); 
                                p.y = r * sin(theta); 
                                // p.z is unchanged
                                break;
                        case SQUAREOFF_TRANSFORM: // f_amt to describe amount of compression amount
                                p.x = p.x / Ts[i].data.f_amt;
                                p.y = p.y * Ts[i].data.f_amt;
                                // p.z is unchanged
                                break;
                        default:
                                exit(1);
                }
        }

        return p;
}


float Mesh :: compute_total_stretch(Transform* Ts, int num_Ts)
{
        float total_stretch;
        int i;

        total_stretch = 1;
        for (i = 0; i < num_Ts; ++i)
        {
                if (Ts[i].type == STRETCH_TRANSFORM)
                        total_stretch *= Ts[i].data.f_amt;
        }

        return total_stretch;
}

float Mesh :: convert_circular_cane_to_addl_triangles(Triangle* triangles, int* num_triangles, Transform* Ts, int num_Ts, Color color, int illuminated_subcane, int res_mode)
{
        Point p1, p2, p3, p4;
        Triangle tmp_t;
        int i, j, angular_resolution, axial_resolution;
        float total_stretch, radius;

        switch (res_mode)
        {
                case LOW_RESOLUTION:
                        angular_resolution = LOW_ANGULAR_RESOLUTION;
                        axial_resolution = LOW_AXIAL_RESOLUTION;
                        break;
                case HIGH_RESOLUTION:
                        angular_resolution = HIGH_ANGULAR_RESOLUTION;
                        axial_resolution = HIGH_AXIAL_RESOLUTION;
                        break;
                default:
                        exit(1);
        }

        
        total_stretch = compute_total_stretch(Ts, num_Ts);
        radius = 0.0;

        // Get cylinder sides
        for (i = 0; i < axial_resolution - 1; ++i)
        {
                for (j = 0; j < angular_resolution; ++j)
                {
                        p1.x = cos(2 * PI * ((float) j) / angular_resolution);
                        p1.y = sin(2 * PI * ((float) j) / angular_resolution);
                        p1.z = ((float) i) / (axial_resolution * total_stretch);

                        p2.x = cos(2 * PI * ((float) j) / angular_resolution);
                        p2.y = sin(2 * PI * ((float) j) / angular_resolution);
                        p2.z = ((float) i+1) / (axial_resolution * total_stretch);

                        p3.x = cos(2 * PI * ((float) j+1) / angular_resolution);
                        p3.y = sin(2 * PI * ((float) j+1) / angular_resolution);
                        p3.z = ((float) i) / (axial_resolution * total_stretch);

                        p4.x = cos(2 * PI * ((float) j+1) / angular_resolution);
                        p4.y = sin(2 * PI * ((float) j+1) / angular_resolution);
                        p4.z = ((float) i+1) / (axial_resolution * total_stretch);

                        p1 = apply_transforms(p1, Ts, num_Ts);
                        p2 = apply_transforms(p2, Ts, num_Ts);
                        p3 = apply_transforms(p3, Ts, num_Ts);
                        p4 = apply_transforms(p4, Ts, num_Ts);

                        radius = MAX(radius, 
                                        MAX(sqrt(p1.x*p1.x + p1.y*p1.y),
                                                MAX(sqrt(p2.x*p2.x + p2.y*p2.y),
                                                        MAX(sqrt(p3.x*p3.x + p3.y*p3.y),
                                                                sqrt(p4.x*p4.x + p4.y*p4.y)))));

                        tmp_t.v1 = p2;
                        tmp_t.v2 = p1;
                        tmp_t.v3 = p4;
                        if (illuminated_subcane == ALL_SUBCANES)
                                tmp_t.c = brighten_color(color);
                        else
                                tmp_t.c = darken_color(color);

                        triangles[*num_triangles] = tmp_t;
                        *num_triangles += 1;

                        tmp_t.v1 = p1;
                        tmp_t.v2 = p3;
                        tmp_t.v3 = p4;
                        if (illuminated_subcane == ALL_SUBCANES)
                                tmp_t.c = brighten_color(color);
                        else
                                tmp_t.c = darken_color(color);

                        triangles[*num_triangles] = tmp_t;
                        *num_triangles += 1;
                } 
        }

        // Get cylinder bottom
        p1.x = 1.0;
        p1.y = 0.0;
        p1.z = p2.z = p3.z = 0.0;
        tmp_t.v1 = apply_transforms(p1, Ts, num_Ts);
        if (illuminated_subcane == ALL_SUBCANES)
                tmp_t.c = brighten_color(color);
        else
                tmp_t.c = darken_color(color);
        for (j = 1; j < angular_resolution-1; ++j)
        {
                p2.x = cos(2 * PI * ((float) j) / angular_resolution);
                p2.y = sin(2 * PI * ((float) j) / angular_resolution);
                tmp_t.v3 = apply_transforms(p2, Ts, num_Ts);
                p3.x = cos(2 * PI * ((float) j+1) / angular_resolution);
                p3.y = sin(2 * PI * ((float) j+1) / angular_resolution);
                tmp_t.v2 = apply_transforms(p3, Ts, num_Ts);
                triangles[*num_triangles] = tmp_t;
                *num_triangles += 1;
        }

        // Get cylinder top
        p1.z = p2.z = p3.z = ((float) (axial_resolution-1)) / (axial_resolution * total_stretch);
        tmp_t.v1 = apply_transforms(p1, Ts, num_Ts);
        for (j = 1; j < angular_resolution-1; ++j)
        {
                p2.x = cos(2 * PI * ((float) j) / angular_resolution);
                p2.y = sin(2 * PI * ((float) j) / angular_resolution);
                tmp_t.v2 = apply_transforms(p2, Ts, num_Ts);
                p3.x = cos(2 * PI * ((float) j+1) / angular_resolution);
                p3.y = sin(2 * PI * ((float) j+1) / angular_resolution);
                tmp_t.v3 = apply_transforms(p3, Ts, num_Ts);
                triangles[*num_triangles] = tmp_t;
                *num_triangles += 1;
        }
        
        return radius; 
}


/*
Assumptions about parameters:

1. c is non-null.
2. The values of c->twist and c->stretch are not *both* non-zero and non-one, respectively.
3. If c->twist or c->stretch are non-zero or non-one respective, then c has exactly one child.
4. Ts is sufficiently long for the cane.
5. illuminated_subcane is equal to neither ALL_SUBCANES nor NO_SUBCANES only if
c->twist = 0.0 and c->stretch = 1.0. 
*/
float Mesh :: generateMesh(Cane* c, Triangle* triangles, int* num_triangles, 
        Transform* Ts, int* num_Ts, int illuminated_subcane, int global_wrap, int res_mode)
{
        int i, illumination;
        float radius;

        if (c == NULL)
                return 0.0;

        switch(c->type)
        {
                case TWIST_CANETYPE:
                        Ts[*num_Ts].type = TWIST_TRANSFORM;
                        Ts[*num_Ts].data.f_amt = c->amt;
                        *num_Ts += 1;
                        radius = generateMesh(c->subcanes[0], triangles, num_triangles, 
                                Ts, num_Ts, illuminated_subcane, global_wrap, res_mode);
                        *num_Ts -= 1;
                        return radius; 
                case BASE_CIRCLE_CANETYPE: 
                        return convert_circular_cane_to_addl_triangles(triangles, num_triangles, 
                                Ts, *num_Ts, c->color, illuminated_subcane, res_mode);
                case SQUAREOFF_CANETYPE: 
                        Ts[*num_Ts].type = SQUAREOFF_TRANSFORM;
                        Ts[*num_Ts].data.f_amt = c->amt;
                        *num_Ts += 1;
                        radius = generateMesh(c->subcanes[0], triangles, num_triangles, 
                                Ts, num_Ts, illuminated_subcane, global_wrap, res_mode);
                        *num_Ts -= 1;
                        return radius;
                case STRETCH_CANETYPE:
                        Ts[*num_Ts].type = STRETCH_TRANSFORM;
                        Ts[*num_Ts].data.f_amt = c->amt;
                        *num_Ts += 1;
                        radius = generateMesh(c->subcanes[0], triangles, num_triangles, 
                                Ts, num_Ts, illuminated_subcane, global_wrap, res_mode);
                        *num_Ts -= 1;
                        return radius;
                case BUNDLE_CANETYPE:
                        radius = 0.0;
                        for (i = 0; i < c->num_subcanes; ++i)
                        {
                                Ts[*num_Ts].type = MOVE_TRANSFORM;
                                Ts[*num_Ts].data.p_amt.x = c->subcane_locs[i].x;
                                Ts[*num_Ts].data.p_amt.y = c->subcane_locs[i].y;
                                *num_Ts += 1;

                                if (i == illuminated_subcane || illuminated_subcane == ALL_SUBCANES)
                                        illumination = ALL_SUBCANES;
                                else
                                        illumination = NO_SUBCANES;

                                radius = MAX(radius, generateMesh(c->subcanes[i], triangles, num_triangles, 
                                        Ts, num_Ts, illumination, 0, res_mode));
                                *num_Ts -= 1;
                        }
                        if (global_wrap)
                        {
                                Transform wrap_Ts[MAX_TRANSFORMS];
                                int num_wrap_Ts;
                                Color wrap_color;

                                wrap_color.r = wrap_color.g = wrap_color.b = 1.0; 
                                for (int i = 0; i < *num_Ts; ++i)
                                {
                                        wrap_Ts[i] = Ts[i];
                                }
                                num_wrap_Ts = *num_Ts; 
                                wrap_Ts[num_wrap_Ts].type = STRETCH_TRANSFORM; 
                                wrap_Ts[num_wrap_Ts].data.f_amt 
                                        = (1.0 / radius) * (1.0 / compute_total_stretch(Ts, *num_Ts));
                                ++num_wrap_Ts;

                                convert_circular_cane_to_addl_triangles(triangles, num_triangles, 
                                        wrap_Ts, num_wrap_Ts, wrap_color, illuminated_subcane, res_mode);
                        }
                        return radius; 
                default:
                        exit(1);
        }
}

Mesh :: Mesh(Cane* c)
{
        low_res_tris = (Triangle*) malloc(sizeof(Triangle) 
                * (LOW_AXIAL_RESOLUTION * LOW_ANGULAR_RESOLUTION * 2) * (MAX_NUM_CANES + 1));
        high_res_tris = (Triangle*) malloc(sizeof(Triangle) 
                * (HIGH_AXIAL_RESOLUTION * HIGH_ANGULAR_RESOLUTION * 2) * (MAX_NUM_CANES + 1));
        
        setCane(c);
}

void Mesh :: setCane(Cane* c)
{
        cane = c;
        illuminated_subcane = NO_SUBCANES;

        updateLowRes();
        updateHighRes();
}

void Mesh :: updateLowRes()
{
        Transform Ts[MAX_TRANSFORMS];
        int num_Ts;

        num_Ts = 0;
        num_low_res_tris = 0;
        generateMesh(cane, low_res_tris, &num_low_res_tris, Ts, &num_Ts, 
                illuminated_subcane, 0, LOW_RESOLUTION);
        low_res_up_to_date = 1;
}

void Mesh :: updateHighRes()
{
        Transform Ts[MAX_TRANSFORMS];
        int num_Ts;

        num_Ts = 0;
        num_high_res_tris = 0;
        generateMesh(cane, high_res_tris, &num_high_res_tris, Ts, &num_Ts, 
                illuminated_subcane, 0, HIGH_RESOLUTION);
        high_res_up_to_date = 1;
}

Cane* Mesh :: getCane()
{
        return cane;
} 

Triangle* Mesh :: getMesh(int resolution)
{

        if (resolution == LOW_RESOLUTION)
        {
                if (!low_res_up_to_date)
                        updateLowRes(); 
                return low_res_tris;
        }
        else
        {
                if (!high_res_up_to_date)
                {
                        updateHighRes();
                        updateLowRes();
                } 
                return high_res_tris;
        }
}

int Mesh :: getNumMeshTriangles(int resolution)
{
        if (resolution == LOW_RESOLUTION)
        {
                if (!low_res_up_to_date)
                        updateLowRes(); 
                return num_low_res_tris;
        }
        else
        {
                if (!high_res_up_to_date)
                        updateHighRes();
                return num_high_res_tris;
        }
}

void Mesh :: twistCane(float amt)
{
        Transform twist_t;

        if (cane == NULL)
                return;
        cane->twist(amt);

        if (!low_res_up_to_date)
                updateLowRes();

        twist_t.type = TWIST_TRANSFORM;
        twist_t.data.f_amt = amt;
        for (int i = 0; i < num_low_res_tris; ++i)
        {
                low_res_tris[i].v1 = apply_transforms(low_res_tris[i].v1, &twist_t, 1);
                low_res_tris[i].v2 = apply_transforms(low_res_tris[i].v2, &twist_t, 1);
                low_res_tris[i].v3 = apply_transforms(low_res_tris[i].v3, &twist_t, 1);
        }

        low_res_up_to_date = 1;
        high_res_up_to_date = 0;
}

void Mesh :: stretchCane(float amt, float max_stretch)
{
        Transform stretch_t;

        if (cane == NULL)
                return;
        cane->stretch(amt, max_stretch);

        if (!low_res_up_to_date)
                updateLowRes();

        stretch_t.type = STRETCH_TRANSFORM;
        stretch_t.data.f_amt = MIN(1.0 + amt, max_stretch);
        for (int i = 0; i < num_low_res_tris; ++i)
        {
                low_res_tris[i].v1 = apply_transforms(low_res_tris[i].v1, &stretch_t, 1);
                low_res_tris[i].v2 = apply_transforms(low_res_tris[i].v2, &stretch_t, 1);
                low_res_tris[i].v3 = apply_transforms(low_res_tris[i].v3, &stretch_t, 1);
        }

        low_res_up_to_date = 1;
        high_res_up_to_date = 0;
}

void Mesh :: squareoffCane(float amt, float max_squareoff)
{
        if (cane == NULL)
                return;
        cane->squareoff(amt, max_squareoff);
        low_res_up_to_date = high_res_up_to_date = 0;
}

void Mesh :: moveCane(int curActiveSubcane, float delta_x, float delta_y)
{
        if (cane == NULL)
                return;
        cane->createBundle();
        cane->subcane_locs[curActiveSubcane].x += delta_x;
        cane->subcane_locs[curActiveSubcane].y += delta_y; 
        low_res_up_to_date = high_res_up_to_date = 0;
}

void Mesh :: addCane(Cane* c, int* active_subcane)
{
        if (cane == NULL)
        {
                cane = c->deepCopy();
        }
        else
        {
                cane->add(c, active_subcane);
        }
        low_res_up_to_date = high_res_up_to_date = 0;
}

void Mesh :: advanceActiveSubcane(int* active_subcane)
{
        *active_subcane += 1;
        *active_subcane %= cane->num_subcanes;
        low_res_up_to_date = high_res_up_to_date = 0;
}

void Mesh :: setIlluminatedSubcane(int new_ill_subcane)
{
        illuminated_subcane = new_ill_subcane;
        low_res_up_to_date = high_res_up_to_date = 0;
}






