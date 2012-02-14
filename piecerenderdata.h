#ifndef PIECERENDERDATA_H
#define PIECERENDERDATA_H

#include "asyncrenderinternal.h"

class Piece;

class PieceRenderData : public RenderData
{
	public:
		PieceRenderData(Piece const *_piece);
		virtual ~PieceRenderData();
		virtual Geometry *getGeometry();
	private:
		Piece *piece;
};

#endif //PIECERENDERDATA_H
