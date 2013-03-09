

#include "pullplangeometrythread.h"
#include "pullplaneditorwidget.h"
#include "pullplan.h"
#include "globalgraphicssetting.h"

PullPlanGeometryThread::PullPlanGeometryThread(PullPlanEditorWidget* _ppew) : ppew(_ppew)
{
	
}

void PullPlanGeometryThread::run()
{
	bool startOver;
	while (1)
	{
		ppew->wakeWait.wait(&(ppew->wakeMutex));

		compute:

		// get lock for ppew's tempPullPlan 
		// and make a copy to get out of his way as fast as possible	
		ppew->tempPullPlanMutex.lock();
		PullPlan* myTempPullPlan = deep_copy(ppew->tempPullPlan);
		ppew->tempPullPlanDirty = false;
		ppew->tempPullPlanMutex.unlock();	
	
		// now lock the geometry
		ppew->tempGeometry1Mutex.lock();
		generateMesh(myTempPullPlan, &(ppew->tempGeometry1), GlobalGraphicsSetting::MEDIUM);
		ppew->tempGeometry1Mutex.unlock();	
		ppew->tempGeometry2Mutex.lock();
		generateMesh(myTempPullPlan, &(ppew->tempGeometry2), GlobalGraphicsSetting::MEDIUM);
		ppew->tempGeometry2Mutex.unlock();	
		ppew->geometryDirtyMutex.lock();
		ppew->geometryDirty = true;
		ppew->geometryDirtyMutex.unlock();

		ppew->tempPullPlanMutex.lock();
		startOver = ppew->tempPullPlanDirty;
		ppew->tempPullPlanMutex.unlock();
		if (startOver)
		{
			deep_delete(myTempPullPlan);
			goto compute;
		}
		
		emit finishedMesh();

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
			ppew->tempPullPlanMutex.lock();
			startOver = ppew->tempPullPlanDirty;
			ppew->tempPullPlanMutex.unlock();
			if (startOver)
			{
				deep_delete(myTempPullPlan);
				goto compute;
			}
		}

		// now lock the geometry
		ppew->tempGeometry1Mutex.lock();
		generateMesh(myTempPullPlan, &(ppew->tempGeometry1), GlobalGraphicsSetting::HIGH);
		ppew->tempGeometry1Mutex.unlock();	
		ppew->tempGeometry2Mutex.lock();
		generateMesh(myTempPullPlan, &(ppew->tempGeometry2), GlobalGraphicsSetting::HIGH);
		ppew->tempGeometry2Mutex.unlock();	
		ppew->geometryDirtyMutex.lock();
		ppew->geometryDirty = true;
		ppew->geometryDirtyMutex.unlock();

		ppew->tempPullPlanMutex.lock();
		startOver = ppew->tempPullPlanDirty;
		ppew->tempPullPlanMutex.unlock();
		if (startOver)
		{
			deep_delete(myTempPullPlan);
			goto compute;
		}
		
		deep_delete(myTempPullPlan);
		emit finishedMesh();
	}
}



