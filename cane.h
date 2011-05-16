

#ifndef CANE_H
#define CANE_H

#include "primitives.h"

typedef struct Cane
{
        float stretch;
        float twist;
        int num_subcanes;
        Point subcane_locs[MAX_SUBCANE_COUNT];
        struct Cane* subcanes[MAX_SUBCANE_COUNT];
        Color color;
} Cane;

Cane* init_cane();

Cane* twist_cane(Cane* c, float radians);

Cane* stretch_cane(Cane* c, float amount, float min_stretch);

Cane* create_bundle(Cane* c);

Cane* add_cane(Cane* c, Cane* addl, int* addl_index_ptr);

Cane* deep_copy(Cane* c);

#endif


