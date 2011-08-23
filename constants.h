
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define LOW_AXIAL_RESOLUTION 100
#define LOW_ANGULAR_RESOLUTION 20
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

#define UNASSIGNED_CANETYPE 0
#define PULL_CANETYPE 1
#define BUNDLE_CANETYPE 2
#define FLATTEN_CANETYPE 3
#define BASE_POLYGONAL_CANETYPE 4

#define CIRCLE 1
#define SQUARE 2
#define HALF_CIRCLE 3

#define MAX_ANCESTORS 20
#define MAX_AMT_TYPES 5
#define MAX_SNAP 30

#define PULL_MODE 2
#define BUNDLE_MODE 4
#define FLATTEN_MODE 5
#define SNAP_MODE 7
#define SNAP_LINE_MODE 8
#define SNAP_CIRCLE_MODE 9

#define FREE_VIEW 1
#define FRONT_VIEW 2
#define TOP_VIEW 3
#define SIDE_VIEW 4

#define NO_SNAP 0
#define SNAP_POINT 1
#define SNAP_LINE 2
#define SNAP_CIRCLE 3

#define PI 3.14159265358979323846f

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define strify(x) #x

#define UNDEFINED "Undefined";

#endif


