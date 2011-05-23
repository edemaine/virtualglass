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

        subcaneCount = 0;
        for (int i = 0; i < MAX_SUBCANE_COUNT; ++i)
        {
                subcanes[i] = NULL;
                subcaneLocations[i].x = subcaneLocations[i].y = 0.0;
        }

        color.r = color.g = color.b = color.a = 1.0;
}

// Returns the number of nodes in the cane's DAG
int Cane :: leafNodes()
{
        if (this->subcaneCount == 0)
        {
                return 1; 
        }
        else
        {
                int total = 0;
                for (int i = 0; i < this->subcaneCount; ++i)
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

        dest->subcaneCount = this->subcaneCount;
        for (int i = 0; i < MAX_SUBCANE_COUNT; ++i)
        {
                if (this->subcanes[i] != NULL)
                {
                        dest->subcanes[i] = this->subcanes[i]->deepCopy();
                        dest->subcaneLocations[i].x = this->subcaneLocations[i].x;
                        dest->subcaneLocations[i].y = this->subcaneLocations[i].y;
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
                this->subcaneCount = 1;
                this->subcanes[0] = copy;
                this->amt = radians;
        }
        else
        {
                this->amt += radians;
        }       
}


void Cane :: stretch(float amount)
{
        if (this->type != STRETCH_CANETYPE)
        {
                Cane* copy = new Cane(UNASSIGNED_CANETYPE);
                this->shallowCopy(copy);
                this->reset();
                this->type = STRETCH_CANETYPE;
                this->subcaneCount = 1;
                this->subcanes[0] = copy;
                this->amt = 1.0;
        }
        this->amt *= (1.0 + amount);
}

void Cane :: squareoff(float amount)
{
        if (this->type != SQUAREOFF_CANETYPE)
        {
                Cane* copy = new Cane(UNASSIGNED_CANETYPE);
                this->shallowCopy(copy);
                this->reset();
                this->type = SQUAREOFF_CANETYPE;
                this->subcaneCount = 1;
                this->subcanes[0] = copy;
                this->amt = 1.0;
        }
        this->amt *= (1.0 + amount);
}

void Cane :: createBundle()
{
        if (this->type != BUNDLE_CANETYPE)
        {
                Cane* copy = new Cane(UNASSIGNED_CANETYPE);
                this->shallowCopy(copy);
                this->reset();
                this->type = BUNDLE_CANETYPE;
                this->subcaneCount = 1;
                this->subcanes[0] = copy;
        }
}

void Cane :: moveCane(int subcane, float delta_x, float delta_y)
{
        this->subcaneLocations[subcane].x += delta_x;
        this->subcaneLocations[subcane].y += delta_y;
}

void Cane :: add(Cane* addl, int* addl_index_ptr)
{
        createBundle();

        // Add the new cane to the bundle
        subcanes[subcaneCount] = addl;
        subcaneLocations[subcaneCount].x = 0;
        subcaneLocations[subcaneCount].y = 0;
        *addl_index_ptr = subcaneCount;
        subcaneCount += 1;
}

Cane* Cane :: deepCopy()
{
        Cane* copy;
        int i;

        copy = new Cane(this->type);
        copy->amt = this->amt;

        copy->subcaneCount = this->subcaneCount;
        for (i = 0; i < MAX_SUBCANE_COUNT; ++i)
        {
                if (this->subcanes[i] != NULL)
                {
                        copy->subcanes[i] = this->subcanes[i]->deepCopy();
                        copy->subcaneLocations[i].x = this->subcaneLocations[i].x;
                        copy->subcaneLocations[i].y = this->subcaneLocations[i].y;
                }
                else
                        copy->subcanes[i] = NULL;
        }
        copy->color = this->color;

        return copy;        
}

