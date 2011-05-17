

#ifndef CANE_H
#define CANE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "primitives.h"

class Cane
{
        public:
                Cane();
                void reset();
                void shallow_copy(Cane* dest);
                void twist_cane(float radians);
                void stretch_cane(float amount, float min_stretch);
                void create_bundle();
                void add_cane(Cane* addl, int* addl_index_ptr);
                Cane* deep_copy();

                float stretch;
                float twist;
                int num_subcanes;
                Point subcane_locs[MAX_SUBCANE_COUNT];
                Cane* subcanes[MAX_SUBCANE_COUNT];
                Color color;
};



#endif


