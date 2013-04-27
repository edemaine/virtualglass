
#include <time.h>

#include "pullplangeometrythread.h"
#include "pullplaneditorwidget.h"
#include "pullplan.h"
#include "globalgraphicssetting.h"

PullPlanGeometryThread::PullPlanGeometryThread(PullPlanEditorWidget* _ppew) : ppew(_ppew)
{
	
}

void PullPlanGeometryThread::run()
{
	bool pullPlanChanged;
	bool completed;
	while (1)
	{
		sleep:

		ppew->wakeWait.wait(&(ppew->wakeMutex));

		compute:

		// get lock for ppew's tempPullPlan 
		// and make a copy to get out of his way as fast as possible	
		ppew->tempPullPlanMutex.lock();
		PullPlan* myTempPullPlan = deep_copy(ppew->tempPullPlan);
		ppew->tempPullPlanDirty = false;
		ppew->tempPullPlanMutex.unlock();	
	
		// now lock the geometry
		ppew->tempGeometryMutex.lock();
		completed = generateMesh(myTempPullPlan, &(ppew->tempGeometry), GlobalGraphicsSetting::MEDIUM);
		ppew->tempGeometryMutex.unlock();	
		ppew->geometryDirtyMutex.lock();
		ppew->geometryDirty = true;
		ppew->geometryDirtyMutex.unlock();

		ppew->tempPullPlanMutex.lock();
		pullPlanChanged = ppew->tempPullPlanDirty;
		ppew->tempPullPlanMutex.unlock();
		if (pullPlanChanged)
		{
			deep_delete(myTempPullPlan);
			goto compute;
		}
		
		emit finishedMesh(completed, GlobalGraphicsSetting::MEDIUM);
		if (!completed)
		{
			deep_delete(myTempPullPlan);
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
			ppew->tempPullPlanMutex.lock();
			pullPlanChanged = ppew->tempPullPlanDirty;
			ppew->tempPullPlanMutex.unlock();
			if (pullPlanChanged)
			{
				deep_delete(myTempPullPlan);
				goto compute;
			}
		}

		// now lock the geometry
		ppew->tempGeometryMutex.lock();
		completed = generateMesh(myTempPullPlan, &(ppew->tempGeometry), GlobalGraphicsSetting::VERY_HIGH);
		ppew->tempGeometryMutex.unlock();	
		ppew->geometryDirtyMutex.lock();
		ppew->geometryDirty = true;
		ppew->geometryDirtyMutex.unlock();

		ppew->tempPullPlanMutex.lock();
		pullPlanChanged = ppew->tempPullPlanDirty;
		ppew->tempPullPlanMutex.unlock();
		if (pullPlanChanged)
		{
			deep_delete(myTempPullPlan);
			goto compute;
		}
		
		deep_delete(myTempPullPlan);
		emit finishedMesh(completed, GlobalGraphicsSetting::VERY_HIGH);
	}
}



