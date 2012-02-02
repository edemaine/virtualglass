
#include "casing.h"

Casing :: Casing() {

	this->thickness = 0.01;
	this->shape = CIRCLE_SHAPE;
}

Casing :: Casing(float thickness, int shape) {

	this->thickness = thickness;
	this->shape = shape;
}



