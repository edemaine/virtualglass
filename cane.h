

#ifndef CANE_H
#define CANE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "primitives.h"
#include "constants.h"
#include "yaml-cpp/yaml.h"

class Cane
{
    public:
        Cane(int type);
        void reset();
        void shallowCopy(Cane* dest);
        void twistAndStretch(float twistRadians, float stretchMultiplier);
        void stretch(float multiplier);
        void twist(float radians);
        void flatten(float rectangle_ratio, float rectangle_theta, float flatness);
        void createBundle();
        void add(Cane* addl, int* addl_index_ptr);
        void moveCane(int subcane, float delta_x, float delta_y);
        int leafNodes();
        Cane* deepCopy();
        void setColor(Color color);
        std::string yamlRepresentation();

        int type;
        float amts[MAX_AMT_TYPES];
        int subcaneCount;
        Point subcaneLocations[MAX_SUBCANE_COUNT];
        Cane* subcanes[MAX_SUBCANE_COUNT];
        Color color;
};



#endif


