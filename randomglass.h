
#ifndef RANDOMGLASS_H
#define RANDOMGLASS_H

#include "shape.h"

class GlassColor;
class PullPlan;
class Pickup;

GlassColor* randomGlassColor();

PullPlan* randomSimplePullPlan(enum GeometricShape outermostCasingShape, GlassColor* color);

PullPlan* randomComplexPullPlan(PullPlan* circleSimplePlan, PullPlan* squareSimplePlan);

Pickup* randomPickup(PullPlan* plan1, PullPlan* plan2=NULL);

Piece* randomPiece(Pickup* pickup);

#endif

