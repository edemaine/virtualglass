
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
	bool startOver;

	while (1)
	{
		pew->wakeWait.wait(&(pew->wakeMutex));
		
		compute:

		// get a copy of the piece
		pew->tempPieceMutex.lock();
		Piece* myTempPiece = deep_copy(pew->tempPiece);
		pew->tempPieceDirty = false;
		pew->tempPieceMutex.unlock();	
		
		// compute the low-res geometry
		pew->tempGeometry1Mutex.lock();
		generateMesh(myTempPiece, &(pew->tempPieceGeometry1), &(pew->tempPickupGeometry1), 
			GlobalGraphicsSetting::VERY_LOW);
		pew->tempGeometry1Mutex.unlock();	
		pew->tempGeometry2Mutex.lock();
		generateMesh(myTempPiece, &(pew->tempPieceGeometry1), &(pew->tempPickupGeometry1), 
			GlobalGraphicsSetting::VERY_LOW);
		pew->tempGeometry2Mutex.unlock();	
		pew->geometryDirtyMutex.lock();
		pew->geometryDirty = true;
		pew->geometryDirtyMutex.unlock();

		// if the piece already changed, start over
		pew->tempPieceMutex.lock();
		startOver = pew->tempPieceDirty;
		pew->tempPieceMutex.unlock();

		if (startOver)
		{
			deep_delete(myTempPiece);
			goto compute;
		}
		emit finishedMesh();

		// now we don't launch immediately into the high-res version
		// the waiting is to avoid annoying flashing, gui blocking
		// (from copying geometry), and wasted effort in producing a
		// high-res geometry that will immediately be replaced.
		//
		unsigned int delay_msecs = 2000;
		for (unsigned int i = 0; i < delay_msecs / 500; ++i)
		{
			msleep(500);
			
			// check if piece has changed and start over if so	
			pew->tempPieceMutex.lock();
			startOver = pew->tempPieceDirty;
			pew->tempPieceMutex.unlock();

			if (startOver)
			{
				deep_delete(myTempPiece);
				goto compute;
			}
		}

		// compute the high-res geometry
		pew->tempGeometry1Mutex.lock();
		generateMesh(myTempPiece, &(pew->tempPieceGeometry1), &(pew->tempPickupGeometry1), 
			GlobalGraphicsSetting::HIGH);
		pew->tempGeometry1Mutex.unlock();	
		pew->tempGeometry2Mutex.lock();
		generateMesh(myTempPiece, &(pew->tempPieceGeometry2), &(pew->tempPickupGeometry2), 
			GlobalGraphicsSetting::HIGH);
		pew->tempGeometry2Mutex.unlock();	
		pew->geometryDirtyMutex.lock();
		pew->geometryDirty = true;
		pew->geometryDirtyMutex.unlock();

		// if the piece changed, start over
		pew->tempPieceMutex.lock();
		startOver = pew->tempPieceDirty;
		pew->tempPieceMutex.unlock();

		if (startOver)
		{
			deep_delete(myTempPiece);
			goto compute;
		}

		deep_delete(myTempPiece);
		emit finishedMesh();
	}
}



