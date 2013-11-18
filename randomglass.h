
#ifndef RANDOMGLASS_H
#define RANDOMGLASS_H

#include "shape.h"

class GlassColor;
class Cane;
class Pickup;

GlassColor* randomGlassColor();

Cane* randomSimpleCane(enum GeometricShape outermostCasingShape, GlassColor* color);

Cane* randomComplexCane(Cane* circleSimpleCane, Cane* squareSimpleCane);

Pickup* randomPickup(Cane* cane1, Cane* cane2=NULL);

Piece* randomPiece(Pickup* pickup);

#endif

