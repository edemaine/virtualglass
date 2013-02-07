

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

		for (unsigned int quality = 1; quality < 5; ++quality)
		{	
			// get lock for ppew's tempPiece 
			// and make a copy to get out of his way as fast as possible	
			ppew->tempPickupMutex.lock();
			PickupPlan* myTempPickup = deep_copy(ppew->tempPickup);
			ppew->tempPickupDirty = false;
			ppew->tempPickupMutex.unlock();	
		
			// now lock the geometry
			ppew->tempGeometry1Mutex.lock();
			generateMesh(myTempPickup, &(ppew->tempGeometry1), quality);
			ppew->tempGeometry1Mutex.unlock();	
			ppew->tempGeometry2Mutex.lock();
			generateMesh(myTempPickup, &(ppew->tempGeometry2), quality);
			ppew->tempGeometry2Mutex.unlock();	
			ppew->geometryDirtyMutex.lock();
			ppew->geometryDirty = true;
			ppew->geometryDirtyMutex.unlock();
			emit finishedMesh();

			ppew->tempPickupMutex.lock();
			bool startOver = ppew->tempPickupDirty;
			ppew->tempPickupMutex.unlock();
			if (startOver)
			{
				deep_delete(myTempPickup);
				quality = 0;	
			}
		}
	}
}



