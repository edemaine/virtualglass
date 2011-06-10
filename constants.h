
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define LOW_AXIAL_RESOLUTION 100
#define LOW_ANGULAR_RESOLUTION 40 
#define HIGH_AXIAL_RESOLUTION 300
#define HIGH_ANGULAR_RESOLUTION 60 
#define MAX_SUBCANE_COUNT 100
#define MAX_FILENAME_LENGTH 200

#define LOW_RESOLUTION 1
#define HIGH_RESOLUTION 2

#define ALL_SUBCANES -1
#define NO_SUBCANES -2

#define UNASSIGNED_CANETYPE 0
#define TWIST_CANETYPE 1
#define STRETCH_CANETYPE 2
#define BUNDLE_CANETYPE 3
#define FLATTEN_CANETYPE 4
#define BASE_CIRCLE_CANETYPE 5

#define MAX_ANCESTORS 20
#define MAX_NUM_CANES 100
#define MAX_AMT_TYPES 5

#define LOOK_MODE 1
#define TWIST_MODE 2
#define STRETCH_MODE 3
#define BUNDLE_MODE 4
#define FLATTEN_MODE 5

#define PI 3.14159265358979323846f

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#endif


