

#ifndef CANE_H
#define CANE_H

#include "types.h"

Cane* init_cane();

Cane* twist_cane(Cane* c, float radians);

Cane* stretch_cane(Cane* c, float amount, float min_stretch);

Cane* create_bundle(Cane* c);

Cane* add_cane(Cane* c, Cane* addl, int* addl_index_ptr);

Cane* deep_copy(Cane* c);

#endif


