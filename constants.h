
#ifndef CONSTANTS_H
#define CONSTANTS_H

// IMPORTANT: The lists of templates/shapes must start
// at 1 and be contiguous. This is used in the UI to make
// conversions from menu items to constants simple.

// Geometric shapes
#define UNSPECIFIED_SHAPE 0
#define CIRCLE_SHAPE 1
#define SQUARE_SHAPE 2
#define AMORPHOUS_SHAPE 3

// Types of MIME data
#define COLOR_BAR_MIME 1
#define PULL_PLAN_MIME 2
#define PIECE_MIME 3

// Editor/nice view modes
#define EMPTY_MODE 0
#define COLORBAR_MODE 1
#define PULLPLAN_MODE 2
#define PIECE_MODE 3
#define PICKUPPLAN_MODE 4

// Numeric constants
#define PI 3.14159265358979323846f
#define TWO_PI 6.2831853071795862f
#define SQRT_TWO 1.41421356
#define SQRT_THREE 1.73205081

// Dependancies
#define IS_DEPENDANCY 1
#define USES_DEPENDANCY 2
#define IS_USED_BY_DEPENDANCY 3

#define SINGLE_FILL_RULE 1
#define GROUP_FILL_RULE 2
#define EVERY_OTHER_FILL_RULE 3
#define EVERY_THIRD_FILL_RULE 4

// Little functions
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define strify(x) #x

#define UNDEFINED "Undefined";

#endif


