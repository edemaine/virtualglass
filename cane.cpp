/*
Initializes a cane node (i.e. a constructed top-level
definition of a cane).
*/

#include "cane.h"

Cane :: Cane()
{
        stretch = 1.0;
        twist = 0.0;
        num_subcanes = 0;
        for (int i = 0; i < MAX_SUBCANE_COUNT; ++i)
        {
                subcanes[i] = NULL;
                subcane_locs[i].x = subcane_locs[i].y = 0.0;
        }

        color.r = color.g = color.b = 1.0;
}

Cane* Cane :: twist_cane(float radians)
{
        if (twist == 0.0)
        {
                Cane* new_root = new Cane();
                new_root->num_subcanes = 1;
                new_root->subcanes[0] = this;
                new_root->twist = radians;
                return new_root;
        }
        else
        {
                twist += radians;
                return this;
        }       
}

Cane* Cane :: stretch_cane(float amount, float max_stretch)
{
        if (stretch == 1.0)
        {
                Cane* new_root = new Cane();
                new_root->num_subcanes = 1;
                new_root->subcanes[0] = this;
                new_root->stretch += MIN(amount, max_stretch);
                return new_root;
        }
        else
        {
                stretch += amount;
                stretch = MIN(stretch, max_stretch);
                return this;
        }
}

Cane* Cane :: create_bundle()
{
        if (stretch != 1.0 || twist != 0.0)
        {
                Cane* new_root = new Cane();
                new_root->num_subcanes = 1;
                new_root->subcanes[0] = this;
                return new_root;
        }
        else
        {
                return this;
        }
}

Cane* Cane :: add_cane(Cane* addl, int* addl_index_ptr)
{
        Cane* cane = this->create_bundle();
        cane->subcanes[cane->num_subcanes] = addl;
        cane->subcane_locs[cane->num_subcanes].x = 0;
        cane->subcane_locs[cane->num_subcanes].y = 0;
        *addl_index_ptr = cane->num_subcanes;
        cane->num_subcanes += 1;

        return cane;
}

Cane* Cane :: deep_copy()
{
        Cane* copy;
        int i;

        copy = new Cane();

        copy->stretch = this->stretch;
        copy->twist = this->twist;

        copy->num_subcanes = this->num_subcanes;
        for (i = 0; i < MAX_SUBCANE_COUNT; ++i)
        {
                if (this->subcanes[i] != NULL)
                {
                        copy->subcanes[i] = this->subcanes[i]->deep_copy();
                        copy->subcane_locs[i].x = this->subcane_locs[i].x;
                        copy->subcane_locs[i].y = this->subcane_locs[i].y;
                }
                else
                        copy->subcanes[i] = NULL;
        }
        copy->color.r = this->color.r;
        copy->color.g = this->color.g;
        copy->color.b = this->color.b;

        return copy;        
}

