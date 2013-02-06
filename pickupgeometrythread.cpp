

#include "pickupgeometrythread.h"
#include "pickupplaneditorviewwidget.h"
#include "pickupplan.h"

PickupGeometryThread::PickupGeometryThread(PickupPlanEditorViewWidget* _ppew) : ppew(_ppew)
{
	
}

void PickupGeometryThread::run()
{
	while (1)
	{
		ppew->wakeWait.wait(&(ppew->wakeMutex));

		start:
	
		// get lock for ppew's tempPiece 
		// and make a copy to get out of his way as fast as possible	
		ppew->tempPickupMutex.lock();
		PickupPlan* myTempPickup = deep_copy(ppew->tempPickup);
		ppew->tempPickupDirty = false;
		ppew->tempPickupMutex.unlock();	
	
		// now lock the geometry
		ppew->tempGeometry1Mutex.lock();
		generateMesh(myTempPickup, &(ppew->tempGeometry1), 0);
		ppew->tempGeometry1Mutex.unlock();	
		ppew->tempGeometry2Mutex.lock();
		generateMesh(myTempPickup, &(ppew->tempGeometry2), 0);
		ppew->tempGeometry2Mutex.unlock();	
		emit finishedMesh();

		ppew->tempPickupMutex.lock();
		bool startOver = ppew->tempPickupDirty;
		ppew->tempPickupMutex.unlock();
		if (startOver)
		{
			deep_delete(myTempPickup);
			goto start;
		}

		ppew->tempGeometry1Mutex.lock();
		generateMesh(myTempPickup, &(ppew->tempGeometry1), UINT_MAX);
		ppew->tempGeometry1Mutex.unlock();	
		ppew->tempGeometry2Mutex.lock();
		generateMesh(myTempPickup, &(ppew->tempGeometry2), UINT_MAX);
		ppew->tempGeometry2Mutex.unlock();	
		emit finishedMesh();
	}
}



