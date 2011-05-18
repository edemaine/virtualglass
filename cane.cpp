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
        amt = 0.0;
        is_root_bundle = 0;

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
void Cane :: shallowCopy(Cane* dest)
{
        dest->type = this->type;
        dest->amt = this->amt;
        dest->is_root_bundle = this->is_root_bundle;

        dest->num_subcanes = this->num_subcanes;
        for (int i = 0; i < MAX_SUBCANE_COUNT; ++i)
        {
                if (this->subcanes[i] != NULL)
                {
                        dest->subcanes[i] = this->subcanes[i]->deepCopy();
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

void Cane :: twist(float radians)
{
        if (this->type != TWIST_CANETYPE)
        {
                Cane* copy = new Cane(UNASSIGNED_CANETYPE);
                this->shallowCopy(copy);
                this->reset();
                this->type = TWIST_CANETYPE;
                this->num_subcanes = 1;
                this->subcanes[0] = copy;
                this->amt = radians;
        }
        else
        {
                this->amt += radians;
        }       
}


void Cane :: stretch(float amount, float max_stretch)
{
        if (this->type != STRETCH_CANETYPE)
        {
                Cane* copy = new Cane(UNASSIGNED_CANETYPE);
                this->shallowCopy(copy);
                this->reset();
                this->type = STRETCH_CANETYPE;
                this->num_subcanes = 1;
                this->subcanes[0] = copy;
                this->amt = 1.0;
        }
        this->amt *= (1.0 + amount);
        this->amt = MIN(this->amt, max_stretch);
}

void Cane :: squareoff(float amount, float max_squareoff)
{
        if (this->type != SQUAREOFF_CANETYPE)
        {
                Cane* copy = new Cane(UNASSIGNED_CANETYPE);
                this->shallowCopy(copy);
                this->reset();
                this->type = SQUAREOFF_CANETYPE;
                this->num_subcanes = 1;
                this->subcanes[0] = copy;
                this->amt = 1.0;
        }
        this->amt += amount;
        this->amt = MIN(this->amt, max_squareoff);
}

void Cane :: createBundle()
{
        if (this->type != BUNDLE_CANETYPE)
        {
                Cane* copy = new Cane(UNASSIGNED_CANETYPE);
                this->shallowCopy(copy);
                this->reset();
                this->type = BUNDLE_CANETYPE;
                this->num_subcanes = 1;
                this->subcanes[0] = copy;
                this->is_root_bundle = 1;
                copy->turnOffRootBundle();
        }
}


// Moves through the entire tree and turns off the flag for being
// the bundle closest to the root (i.e. the root bundle).
void Cane :: turnOffRootBundle()
{
        this->is_root_bundle = 0;
        for (int i = 0; i < num_subcanes; ++i)
        {
                subcanes[i]->turnOffRootBundle();
        }
}

void Cane :: add(Cane* addl, int* addl_index_ptr)
{
        // Create a root bundle node
        createBundle();

        // Add the new cane to the bundle
        subcanes[num_subcanes] = addl;
        subcane_locs[num_subcanes].x = 0;
        subcane_locs[num_subcanes].y = 0;
        *addl_index_ptr = num_subcanes;
        num_subcanes += 1;
}

Cane* Cane :: deepCopy()
{
        Cane* copy;
        int i;

        copy = new Cane(this->type);
        copy->amt = this->amt;
        copy->is_root_bundle = this->is_root_bundle;

        copy->num_subcanes = this->num_subcanes;
        for (i = 0; i < MAX_SUBCANE_COUNT; ++i)
        {
                if (this->subcanes[i] != NULL)
                {
                        copy->subcanes[i] = this->subcanes[i]->deepCopy();
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

