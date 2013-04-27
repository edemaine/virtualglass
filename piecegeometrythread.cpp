
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
		sleep:

		pew->wakeWait.wait(&(pew->wakeMutex));
		
		compute:

		// get a copy of the piece
		pew->tempPieceMutex.lock();
		Piece* myTempPiece = deep_copy(pew->tempPiece);
		pew->tempPieceDirty = false;
		pew->tempPieceMutex.unlock();	
		
		// compute the low-res geometry
		pew->tempGeometryMutex.lock();
		completed = generateMesh(myTempPiece, &(pew->tempPieceGeometry), &(pew->tempPickupGeometry), 
			GlobalGraphicsSetting::VERY_LOW);
		pew->tempGeometryMutex.unlock();	
		pew->geometryDirtyMutex.lock();
		pew->geometryDirty = true;
		pew->geometryDirtyMutex.unlock();

		// if the piece already changed, start over
		pew->tempPieceMutex.lock();
		pieceChanged = pew->tempPieceDirty;
		pew->tempPieceMutex.unlock();

		if (pieceChanged)
		{
			deep_delete(myTempPiece);
			goto compute;
		}

		emit finishedMesh(completed, GlobalGraphicsSetting::VERY_LOW);
		if (!completed)
		{
			deep_delete(myTempPiece);
			goto sleep;
		}

		// now we don't launch immediately into the high-res version
		// the waiting is to avoid annoying flashing, gui blocking
		// (from copying geometry), and wasted effort in producing a
		// high-res geometry that will immediately be replaced.
		//
		unsigned int delay_msecs = 2000;
		for (unsigned int i = 0; i < delay_msecs / 250; ++i)
		{
			msleep(250);
			
			// check if piece has changed and start over if so	
			pew->tempPieceMutex.lock();
			pieceChanged = pew->tempPieceDirty;
			pew->tempPieceMutex.unlock();

			if (pieceChanged)
			{
				deep_delete(myTempPiece);
				goto compute;
			}
		}

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

		deep_delete(myTempPiece);
		emit finishedMesh(completed, GlobalGraphicsSetting::HIGH);
	}
}



