

#ifndef CANE_H
#define CANE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "primitives.h"
#include "constants.h"

class Cane
{
        public:
                Cane(int type);
                void reset();
                void shallowCopy(Cane* dest);
                void twist(float radians);
                void stretch(float amount, float min_stretch);
                void squareoff(float amount, float min_stretch);
                void createBundle();
                void add(Cane* addl, int* addl_index_ptr);
                int hasBundle();
                Cane* deepCopy();

                int type;
                float amt;
                int num_subcanes;
                Point subcane_locs[MAX_SUBCANE_COUNT];
                Cane* subcanes[MAX_SUBCANE_COUNT];
                Color color;
};



#endif


