

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

		for (unsigned int i = 0; i < 2; ++i)
		{	
			unsigned int quality;
			if (i == 0)
				quality = GlobalGraphicsSetting::VERY_LOW;
			else
				quality = GlobalGraphicsSetting::HIGH;

			// get lock for ppew's tempPullPlan 
			// and make a copy to get out of his way as fast as possible	
			ppew->tempPullPlanMutex.lock();
			PullPlan* myTempPullPlan = deep_copy(ppew->tempPullPlan);
			ppew->tempPullPlanDirty = false;
			ppew->tempPullPlanMutex.unlock();	
		
			// now lock the geometry
			ppew->tempGeometry1Mutex.lock();
			generateMesh(myTempPullPlan, &(ppew->tempGeometry1), quality);
			ppew->tempGeometry1Mutex.unlock();	
			ppew->tempGeometry2Mutex.lock();
			generateMesh(myTempPullPlan, &(ppew->tempGeometry2), quality);
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



