
#ifndef PIECECROSSECTIONRENDER_H
#define PIECECROSSECTIONRENDER_H

#include "primitives.h"
#include "shape.h"

class QPainter;
class Piece;

namespace PieceCrossSectionRender
{
	void render(QPainter* painter, int size, Piece* piece);
}

#endif

