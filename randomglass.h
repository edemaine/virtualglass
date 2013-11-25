
#ifndef RANDOMGLASS_H
#define RANDOMGLASS_H

#include "shape.h"

class GlassColor;
class Cane;
class Piece;

GlassColor* randomGlassColor();

Cane* randomSimpleCane(enum GeometricShape outermostCasingShape, GlassColor* color);

Cane* randomComplexCane(Cane* circleSimpleCane, Cane* squareSimpleCane);

Piece* randomPiece(Cane* cane1, Cane* cane2=NULL);

#endif

