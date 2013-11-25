
#include <time.h>

#include "piecegeometrythread.h"
#include "pieceeditorwidget.h"
#include "piece.h"
#include "globalgraphicssetting.h"
#include "mesh.h"

PieceGeometryThread::PieceGeometryThread(PieceEditorWidget* _pew) : pew(_pew)
{
	
}

void PieceGeometryThread::run()
{
	bool pieceChanged;
	bool completed;

	while (1)
	{
		pew->wakeWait.wait(&(pew->wakeMutex));
		
		compute:

		// get a copy of the piece
		pew->tempPieceMutex.lock();
		Piece* myTempPiece = deep_copy(pew->tempPiece_);
		pew->tempPieceDirty = false;
		pew->tempPieceMutex.unlock();	
		
		// compute the high-res geometry
		pew->tempGeometryMutex.lock();
		completed = generateMesh(myTempPiece, &(pew->tempPieceGeometry), &(pew->tempPickupGeometry), 
			GlobalGraphicsSetting::HIGH);
		pew->tempGeometryMutex.unlock();	
		pew->geometryDirtyMutex.lock();
		pew->geometryDirty = true;
		pew->geometryDirtyMutex.unlock();

		// if the piece changed, start over
		pew->tempPieceMutex.lock();
		pieceChanged = pew->tempPieceDirty;
		pew->tempPieceMutex.unlock();
		if (pieceChanged)
		{
			deep_delete(myTempPiece);
			goto compute;
		}

		// finish and loop, going to sleep
		deep_delete(myTempPiece);
		emit finishedMesh(completed, GlobalGraphicsSetting::HIGH);
	}
}



