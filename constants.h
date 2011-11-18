
#ifndef CONSTANTS_H
#define CONSTANTS_H

// IMPORTANT: The lists of templates/shapes must start
// at 1 and be contiguous. This is used in the UI to make
// conversions from menu items to constants simple.

// Geometric shapes
#define CIRCLE_SHAPE 1
#define SQUARE_SHAPE 2

// Types of MIME data
#define PULL_PLAN_MIME 1
#define PICKUP_PLAN_MIME 2
#define PIECE_MIME 3

// Editor/nice view modes
#define EMPTY_MODE 0
#define COLORBAR_MODE 1
#define PULLPLAN_MODE 2
#define PIECE_MODE 3
#define PICKUPPLAN_MODE 4

#define PI 3.14159265358979323846f
#define TWO_PI 6.2831853071795862f

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define strify(x) #x

#define UNDEFINED "Undefined";

#endif


