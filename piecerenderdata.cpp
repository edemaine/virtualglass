
#include "piecerenderdata.h"
#include "piece.h"
#include "mesh.h"

PieceRenderData::PieceRenderData(Piece const *_piece) : piece(deep_copy(_piece))
{
}


PieceRenderData::~PieceRenderData()
{
	deep_delete(piece);
}

Geometry *PieceRenderData::getGeometry()
{
	Geometry *geometry = new Geometry;
	Mesher mesher;
	mesher.generateMesh(piece, geometry, false);
	return geometry;
}
