
#ifndef CONSTANTS_H
#define CONSTANTS_H

// IMPORTANT: The lists of templates/shapes must start
// at 1 and be contiguous. This is used in the UI to make
// conversions from menu items to constants simple.

// Geometric shapes
#define CIRCLE_SHAPE 1
#define SQUARE_SHAPE 2
#define AMORPHOUS_SHAPE 3

// Pickup cane orientations
#define HORIZONTAL_ORIENTATION 1
#define VERTICAL_ORIENTATION 2

// Types of MIME data
#define PULL_PLAN_MIME 1
#define PICKUP_PLAN_MIME 2
#define PIECE_MIME 3

// Editor/nice view modes
#define PULLPLAN_MODE 0
#define PICKUPPLAN_MODE 1
#define PIECE_MODE 2

#define PI 3.14159265358979323846f

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define strify(x) #x

#define UNDEFINED "Undefined";

#endif


