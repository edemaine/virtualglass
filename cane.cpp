/*
Initializes a cane node (i.e. a constructed top-level
definition of a cane).
*/

#include "cane.h"
#include <QMessageBox>

Cane :: Cane(int type)
{
        reset();
        this->type = type;
}

// Resets the object to the default values of everything 
void Cane :: reset()
{
        int i;

        type = UNASSIGNED_CANETYPE;
        for (i = 0; i < MAX_AMT_TYPES; ++i)
        {        
                amts[i] = 0.0;
        }

        subcaneCount = 0;
        for (i = 0; i < MAX_SUBCANE_COUNT; ++i)
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
        int i;

        dest->type = this->type;
        for (i = 0; i < MAX_AMT_TYPES; ++i)
        {
                dest->amts[i] = this->amts[i]; 
        }

        dest->subcaneCount = this->subcaneCount;
        for (i = 0; i < MAX_SUBCANE_COUNT; ++i)
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
                this->amts[0] = radians;
        }
        else
        {
                this->amts[0] += radians;
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
                this->amts[0] = 1.0;
        }
        this->amts[0] *= (1.0 + amount);
}

/*
Cane::flatten() creates a new root node that deforms the cane
from a circular shape to an approximation of a rectangle.
The `rectangle_ratio' specifies the relative dimensions of
the rectangle the cane will be flattened into. A ratio of
1.0 is square, 2.0 is a rectangle twice as wide as it is tall, etc.
`rectangle_theta' specifies the orientation of the x-axis of the 
rectangle relative to the global cane x-axis. `flatness' specifies
how closely the cane is squished into the goal rectangle. A
flatness of 0 means the cane remains circular, while a ratio of 1 means
the cane is deformed into a perfect rectangle.
*/
void Cane :: flatten(float rectangle_ratio, float rectangle_theta, float flatness)
{
        if (this->type != FLATTEN_CANETYPE || this->amts[1] != rectangle_theta)
        {
                Cane* copy = new Cane(UNASSIGNED_CANETYPE);
                this->shallowCopy(copy);
                this->reset();
                this->type = FLATTEN_CANETYPE;
                this->subcaneCount = 1;
                this->subcanes[0] = copy;
                this->amts[0] = 1.0; // rectangle_ratio
                this->amts[1] = rectangle_theta;
                this->amts[2] = 0.0; // flatness
        }
        this->amts[0] *= (1.0 + rectangle_ratio);
        this->amts[2] += flatness;
        this->amts[2] = MIN(1.0, MAX(0.0, amts[2]));
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
        int i;
        Cane* copy;

        copy = new Cane(this->type);

        for (i = 0; i < MAX_AMT_TYPES; ++i)
        {
                copy->amts[i] = this->amts[i];
        }

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

void Cane :: setColor(Color color)
{
    this->color = color;
}
