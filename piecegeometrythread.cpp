
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
	while (1)
	{
		pew->wakeWait.wait(&(pew->wakeMutex));

		for (unsigned int quality = 1; quality <= GlobalGraphicsSetting::getQuality(); ++quality)
		{	
			// get lock for pew's tempPiece 
			// and make a copy to get out of his way as fast as possible	
			pew->tempPieceMutex.lock();
			Piece* myTempPiece = deep_copy(pew->tempPiece);
			pew->tempPieceDirty = false;
			pew->tempPieceMutex.unlock();	
		
			// now lock the geometry
			pew->tempGeometry1Mutex.lock();
			generateMesh(myTempPiece, &(pew->tempPieceGeometry1), &(pew->tempPickupGeometry1), quality);
			pew->tempGeometry1Mutex.unlock();	
			pew->tempGeometry2Mutex.lock();
			generateMesh(myTempPiece, &(pew->tempPieceGeometry2), &(pew->tempPickupGeometry2), quality);
			pew->tempGeometry2Mutex.unlock();	
			pew->geometryDirtyMutex.lock();
			pew->geometryDirty = true;
			pew->geometryDirtyMutex.unlock();
			emit finishedMesh();

			pew->tempPieceMutex.lock();
			bool startOver = pew->tempPieceDirty;
			pew->tempPieceMutex.unlock();
			if (startOver)
			{
				deep_delete(myTempPiece);
				quality = 0;	
			}
		}
	}
}



