/*
Initializes a cane node (i.e. a constructed top-level
definition of a cane).
*/

#include "cane.h"

Cane :: Cane(int type)
{
        reset();
        this->type = type;
}

// Resets the object to the default values of everything 
void Cane :: reset()
{
        type = UNASSIGNED_CANETYPE;
        stretch = 1.0;
        twist = 0.0;
        squareoff = 1.0;
        num_subcanes = 0;
        for (int i = 0; i < MAX_SUBCANE_COUNT; ++i)
        {
                subcanes[i] = NULL;
                subcane_locs[i].x = subcane_locs[i].y = 0.0;
        }

        color.r = color.g = color.b = 1.0;
}

// Copies the information in a cane object into 
// the destination cane object
void Cane :: shallow_copy(Cane* dest)
{
        dest->type = this->type;
        dest->stretch = this->stretch;
        dest->twist = this->twist;
        dest->squareoff = this->squareoff;

        dest->num_subcanes = this->num_subcanes;
        for (int i = 0; i < MAX_SUBCANE_COUNT; ++i)
        {
                if (this->subcanes[i] != NULL)
                {
                        dest->subcanes[i] = this->subcanes[i]->deep_copy();
                        dest->subcane_locs[i].x = this->subcane_locs[i].x;
                        dest->subcane_locs[i].y = this->subcane_locs[i].y;
                }
                else
                        dest->subcanes[i] = NULL;
        }
        dest->color.r = this->color.r;
        dest->color.g = this->color.g;
        dest->color.b = this->color.b;
}

void Cane :: twist_cane(float radians)
{
        if (this->type != TWIST_CANETYPE)
        {
                Cane* copy = new Cane(UNASSIGNED_CANETYPE);
                this->shallow_copy(copy);
                this->reset();
                this->type = TWIST_CANETYPE;
                this->num_subcanes = 1;
                this->subcanes[0] = copy;
                this->twist = radians;
        }
        else
        {
                twist += radians;
        }       
}


void Cane :: stretch_cane(float amount, float max_stretch)
{
        if (this->type != STRETCH_CANETYPE)
        {
                Cane* copy = new Cane(UNASSIGNED_CANETYPE);
                this->shallow_copy(copy);
                this->reset();
                this->type = STRETCH_CANETYPE;
                this->num_subcanes = 1;
                this->subcanes[0] = copy;
                this->stretch += MIN(amount, max_stretch);
        }
        else
        {
                stretch += amount;
                stretch = MIN(stretch, max_stretch);
        }
}

void Cane :: squareoff_cane(float amount, float max_squareoff)
{
        if (this->type != SQUAREOFF_CANETYPE)
        {
                Cane* copy = new Cane(UNASSIGNED_CANETYPE);
                this->shallow_copy(copy);
                this->reset();
                this->type = SQUAREOFF_CANETYPE;
                this->num_subcanes = 1;
                this->subcanes[0] = copy;
                this->squareoff += MIN(amount, max_squareoff);
        }
        else
        {
                squareoff += amount;
                squareoff = MIN(squareoff, max_squareoff);
        }
}

void Cane :: create_bundle()
{
        if (this->type != BUNDLE_CANETYPE)
        {
                Cane* copy = new Cane(UNASSIGNED_CANETYPE);
                this->shallow_copy(copy);
                this->reset();
                this->type = BUNDLE_CANETYPE;
                this->num_subcanes = 1;
                this->subcanes[0] = copy;
        }
}

void Cane :: add_cane(Cane* addl, int* addl_index_ptr)
{
        create_bundle();
        subcanes[num_subcanes] = addl;
        subcane_locs[num_subcanes].x = 0;
        subcane_locs[num_subcanes].y = 0;
        *addl_index_ptr = num_subcanes;
        num_subcanes += 1;
}

Cane* Cane :: deep_copy()
{
        Cane* copy;
        int i;

        copy = new Cane(this->type);
        copy->stretch = this->stretch;
        copy->twist = this->twist;
        copy->squareoff = this->squareoff;

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

