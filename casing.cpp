
#include "casing.h"

Casing :: Casing() {

	this->thickness = 0.01;
	this->shape = CIRCLE_SHAPE;
	this->color = new Color();
	color->r = color->g = color->b = 1.0;
	color->a = 0.0;
}

Casing :: Casing(float thickness, int shape, Color* color) {

	this->thickness = thickness;
	this->shape = shape;
	this->color = color;
}



