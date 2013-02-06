

#include "piecegeometrythread.h"
#include "pieceeditorwidget.h"
#include "piece.h"

PieceGeometryThread::PieceGeometryThread(PieceEditorWidget* _pew) : pew(_pew)
{
	
}

void PieceGeometryThread::run()
{
	while (1)
	{
		pew->wakeWait.wait(&(pew->wakeMutex));

		start:
	
		// get lock for pew's tempPiece 
		// and make a copy to get out of his way as fast as possible	
		pew->tempPieceMutex.lock();
		Piece* myTempPiece = deep_copy(pew->tempPiece);
		pew->tempPieceDirty = false;
		pew->tempPieceMutex.unlock();	
	
		// now lock the geometry
		pew->tempGeometry1Mutex.lock();
		generateMesh(myTempPiece, &(pew->tempGeometry1), 0);
		pew->tempGeometry1Mutex.unlock();	
		pew->tempGeometry2Mutex.lock();
		generateMesh(myTempPiece, &(pew->tempGeometry2), 0);
		pew->tempGeometry2Mutex.unlock();	
		emit finishedMesh();

		pew->tempPieceMutex.lock();
		bool startOver = pew->tempPieceDirty;
		pew->tempPieceMutex.unlock();
		if (startOver)
		{
			deep_delete(myTempPiece);
			goto start;
		}

		pew->tempGeometry1Mutex.lock();
		generateMesh(myTempPiece, &(pew->tempGeometry1), UINT_MAX);
		pew->tempGeometry1Mutex.unlock();	
		pew->tempGeometry2Mutex.lock();
		generateMesh(myTempPiece, &(pew->tempGeometry2), UINT_MAX);
		pew->tempGeometry2Mutex.unlock();	
		emit finishedMesh();
	}
}



