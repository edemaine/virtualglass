
#include <time.h>

#include "canegeometrythread.h"
#include "pullplaneditorwidget.h"
#include "pullplan.h"
#include "globalgraphicssetting.h"

CaneGeometryThread::CaneGeometryThread(PullPlanEditorWidget* _ppew) : ppew(_ppew)
{
	
}

void CaneGeometryThread::run()
{
	bool pullPlanChanged;
	bool completed;
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



