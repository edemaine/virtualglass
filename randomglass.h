
#ifndef RANDOMGLASS_H
#define RANDOMGLASS_H

#include <QtGlobal>
#include "glasscolor.h"
#include "pullplan.h"
#include "pulltemplate.h"
#include "pickupplan.h"
#include "pickuptemplate.h"
#include "piece.h"
#include "piecetemplate.h"
#include "shape.h"

GlassColor* randomGlassColor();

PullPlan* randomSimplePullPlan(enum GeometricShape outermostCasingShape, GlassColor* color);

PullPlan* randomComplexPullPlan(PullPlan* circleSimplePlan, PullPlan* squareSimplePlan);

PickupPlan* randomPickup(PullPlan* plan);

Piece* randomPiece(PickupPlan* pickup);

#endif

