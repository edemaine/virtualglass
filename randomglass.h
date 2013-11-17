
#ifndef RANDOMGLASS_H
#define RANDOMGLASS_H

#include "shape.h"

class GlassColor;
class Cane;
class Pickup;

GlassColor* randomGlassColor();

Cane* randomSimpleCane(enum GeometricShape outermostCasingShape, GlassColor* color);

Cane* randomComplexCane(Cane* circleSimplePlan, Cane* squareSimplePlan);

Pickup* randomPickup(Cane* plan1, Cane* plan2=NULL);

Piece* randomPiece(Pickup* pickup);

#endif

