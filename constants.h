
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define LOW_AXIAL_RESOLUTION 100
#define LOW_ANGULAR_RESOLUTION 24
#define HIGH_AXIAL_RESOLUTION 300
#define HIGH_ANGULAR_RESOLUTION 60
#define MAX_SUBCANE_COUNT 20
#define MAX_FILENAME_LENGTH 200

#define LOW_RESOLUTION 1
#define HIGH_RESOLUTION 2

#define ALL_SUBCANES -1
#define NO_SUBCANES -2

#define PERSPECTIVE_PROJECTION 1
#define ORTHOGRAPHIC_PROJECTION 2


// Valid shapes are assumed to start at 1 and be contiguous
// up to some index. Some code uses this constraint. 
#define UNASSIGNED_CANETYPE 0
#define PULL_CANETYPE 1
#define BUNDLE_CANETYPE 2
#define FLATTEN_CANETYPE 3
#define BASE_POLYGONAL_CANETYPE 4

#define UNDEFINED_SHAPE 0
#define CIRCLE_SHAPE 1
#define HALF_CIRCLE_SHAPE 2
#define THIRD_CIRCLE_SHAPE 3
#define SQUARE_SHAPE 4
#define RECTANGLE_SHAPE 5
#define TRIANGLE_SHAPE 6

#define MAX_ANCESTORS 20
#define MAX_AMT_TYPES 5

#define PULL_MODE 2
#define BUNDLE_MODE 4
#define FLATTEN_MODE 5

#define FREE_VIEW 1
#define FRONT_VIEW 2
#define TOP_VIEW 3
#define SIDE_VIEW 4

#define PI 3.14159265358979323846f

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define strify(x) #x

#define UNDEFINED "Undefined";

#endif


