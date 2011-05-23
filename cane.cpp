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

        num_subcanes = 0;
        for (int i = 0; i < MAX_SUBCANE_COUNT; ++i)
        {
                subcanes[i] = NULL;
                subcane_locs[i].x = subcane_locs[i].y = 0.0;
        }

        color.r = color.g = color.b = color.a = 1.0;
}

// Returns the number of nodes in the cane's DAG
int Cane :: leafNodes()
{
        if (this->num_subcanes == 0)
        {
                return 1; 
        }
        else
        {
                int total = 0;
                for (int i = 0; i < this->num_subcanes; ++i)
                        total += this->subcanes[i]->leafNodes();
                return total;
        }
}

// Copies the information in a cane object into 
// the destination cane object
void Cane :: shallowCopy(Cane* dest)
{
        dest->type = this->type;
        dest->amt = this->amt;

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
        dest->color = this->color;
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
        this->amt *= (1.0 + amount);
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
        }
}

void Cane :: moveCane(int subcane, float delta_x, float delta_y)
{
        this->subcane_locs[subcane].x += delta_x;
        this->subcane_locs[subcane].y += delta_y;
}

void Cane :: add(Cane* addl, int* addl_index_ptr)
{
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
        copy->color = this->color;

        return copy;        
}

