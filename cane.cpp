/*
Initializes a cane node (i.e. a constructed top-level
definition of a cane).
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "primitives.h"
#include "cane.h"
#include "constants.h"

#define PI 3.14159265358979323846f

#define MIN(a, b) (((a) < (b)) ? (a) : (b))


Cane* init_cane()
{
        Cane* c;
        int i;

        c = (Cane*) malloc(sizeof(Cane));

        c->stretch = 1.0;
        c->twist = 0.0;
        c->num_subcanes = 0;
        for (i = 0; i < MAX_SUBCANE_COUNT; ++i)
        {
                c->subcanes[i] = NULL;
                c->subcane_locs[i].x = c->subcane_locs[i].y = 0;
        }
        c->color.r = 1.0;
        c->color.g = 1.0;
        c->color.b = 1.0;

        return c;
}

Cane* twist_cane(Cane* c, float radians)
{
        if (c == NULL)
                return c;

        if (c->twist == 0.0)
        {
                Cane* tmp = init_cane();
                tmp->num_subcanes = 1;
                tmp->subcanes[0] = c;
                c = tmp;                 
        }

        c->twist += radians;

        return c;
}

Cane* stretch_cane(Cane* c, float amount, float max_stretch)
{
        if (c == NULL)
                return c;

        if (c->stretch == 1.0)
        {
                Cane* tmp = init_cane();
                tmp->num_subcanes = 1;
                tmp->subcanes[0] = c;
                c = tmp;                 
        }

        c->stretch += amount;
        c->stretch = MIN(c->stretch, max_stretch);

        return c;
}

Cane* create_bundle(Cane* c)
{
        Cane* root;

        if (c == NULL)
        {
                return init_cane();
        }

        if (c->stretch != 1.0 || c->twist != 0.0)
        {
                root = init_cane();
                root->num_subcanes = 1;
                root->subcanes[0] = c;
        }
        else
        {
                root = c;
        }

        return root;
}

Cane* add_cane(Cane* c, Cane* addl, int* addl_index_ptr)
{
        Cane* cane = create_bundle(c);
        cane->subcanes[cane->num_subcanes] = addl;
        cane->subcane_locs[cane->num_subcanes].x = 0;
        cane->subcane_locs[cane->num_subcanes].y = 0;
        *addl_index_ptr = cane->num_subcanes;
        cane->num_subcanes += 1;

        return cane;
}

Cane* deep_copy(Cane* c)
{
        Cane* copy;
        int i;

        copy = init_cane();

        copy->stretch = c->stretch;
        copy->twist = c->twist;

        copy->num_subcanes = c->num_subcanes;
        for (i = 0; i < MAX_SUBCANE_COUNT; ++i)
        {
                if (c->subcanes[i] != NULL)
                {
                        copy->subcanes[i] = deep_copy(c->subcanes[i]);
                        copy->subcane_locs[i].x = c->subcane_locs[i].x;
                        copy->subcane_locs[i].y = c->subcane_locs[i].y;
                }
                else
                        copy->subcanes[i] = NULL;
        }
        copy->color.r = c->color.r;
        copy->color.g = c->color.g;
        copy->color.b = c->color.b;

        return copy;        
}

