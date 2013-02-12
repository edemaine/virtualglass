

#include "pullplangeometrythread.h"
#include "pullplaneditorwidget.h"
#include "pullplan.h"
#include "globalgraphicssetting.h"

PullPlanGeometryThread::PullPlanGeometryThread(PullPlanEditorWidget* _ppew) : ppew(_ppew)
{
	
}

void PullPlanGeometryThread::run()
{
	while (1)
	{
		ppew->wakeWait.wait(&(ppew->wakeMutex));

		for (unsigned int quality = 1; quality <= GlobalGraphicsSetting::getQuality(); ++quality)
		{	
			// get lock for ppew's tempPullPlan 
			// and make a copy to get out of his way as fast as possible	
			ppew->tempPullPlanMutex.lock();
			PullPlan* myTempPullPlan = deep_copy(ppew->tempPullPlan);
			ppew->tempPullPlanDirty = false;
			ppew->tempPullPlanMutex.unlock();	
		
			// now lock the geometry
			ppew->tempGeometry1Mutex.lock();
			generatePullMesh(myTempPullPlan, &(ppew->tempGeometry1), quality);
			ppew->tempGeometry1Mutex.unlock();	
			ppew->tempGeometry2Mutex.lock();
			generatePullMesh(myTempPullPlan, &(ppew->tempGeometry2), quality);
			ppew->tempGeometry2Mutex.unlock();	
                        ppew->geometryDirtyMutex.lock();
                        ppew->geometryDirty = true;
                        ppew->geometryDirtyMutex.unlock();
			emit finishedMesh();

			ppew->tempPullPlanMutex.lock();
			bool startOver = ppew->tempPullPlanDirty;
			ppew->tempPullPlanMutex.unlock();
			if (startOver)
			{
				deep_delete(myTempPullPlan);
				quality = 1;	
			}
		}
	}
}



