
#include "piecerenderdata.h"
#include "piece.h"
#include "mesh.h"
#include "globalgraphicssetting.h"

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
	generateMesh(piece, geometry, NULL, GlobalGraphicsSetting::HIGH);
	return geometry;
}
