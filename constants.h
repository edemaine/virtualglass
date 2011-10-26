
#ifndef CONSTANTS_H
#define CONSTANTS_H

// IMPORTANT: The lists of templates/shapes must start
// at 1 and be contiguous. This is used in the UI to make
// conversions from menu items to constants simple.

// Pull template types
#define LINE_THREE_CIRCLES_TEMPLATE 1
#define LINE_FIVE_CIRCLES_TEMPLATE 2
#define SQUARE_FOUR_CIRCLES_TEMPLATE 3
#define X_NINE_CIRCLES_TEMPLATE 4
#define SQUARE_FOUR_SQUARES_TEMPLATE 5
#define CIRCLE_BASE_TEMPLATE 6
#define SQUARE_BASE_TEMPLATE 7
#define AMORPHOUS_BASE_TEMPLATE 8

// Pickup template types
#define TEN_HORIZONTALS_TEMPLATE 1
#define FIVE_AND_FIVE_HORIZONTALS_TEMPLATE 2
#define TEN_VERTICALS_TEMPLATE 3
#define FIVE_AND_FIVE_VERTICALS_TEMPLATE 4
#define SIX_COLUMNS_OF_TEN_HORIZONTALS_TEMPLATE 5

// Geometric shapes
#define CIRCLE_SHAPE 1
#define SQUARE_SHAPE 2
#define AMORPHOUS_SHAPE 3

// Pickup cane orientations
#define HORIZONTAL_ORIENTATION 1
#define VERTICAL_ORIENTATION 2

#define PI 3.14159265358979323846f

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define strify(x) #x

#define UNDEFINED "Undefined";

#endif


