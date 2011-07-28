

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
	void pullIntuitive(float twistFactor, float stretchFactor);
	void pullIntuitive(int subcane, float twistFactor, float stretchFactor);
	void pullLinear(float twistFactor, float stretchFactor);
	void flatten(float rectangle_ratio, float rectangle_theta, float flatness);
	void flatten(int subcane, float rectangle_ratio, float rectangle_theta, float flatness);
	void createBundle();
	void add(Cane* addl);
	void deleteCane(int subcane);
	void moveCane(int subcane, float delta_x, float delta_y);
	void moveCane(int subcane, float delta_z);
	int leafNodes();
	Cane* deepCopy();
	void setColor(Color color);
	std::string yamlRepresentation();

	int type;
	float amts[MAX_AMT_TYPES];
	int subcaneCount;
	Point subcaneLocations[MAX_SUBCANE_COUNT]; // z is center displacement
	Cane* subcanes[MAX_SUBCANE_COUNT];
	Color color;
};



#endif


