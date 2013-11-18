
#include <time.h>

#include "canegeometrythread.h"
#include "caneeditorwidget.h"
#include "cane.h"
#include "globalgraphicssetting.h"

CaneGeometryThread::CaneGeometryThread(CaneEditorWidget* _ppew) : ppew(_ppew)
{
	
}

void CaneGeometryThread::run()
{
	bool caneChanged;
	bool completed;
	while (1)
	{
		ppew->wakeWait.wait(&(ppew->wakeMutex));

		compute:

		// get lock for ppew's tempCane 
		// and make a copy to get out of his way as fast as possible	
		ppew->tempCaneMutex.lock();
		Cane* myTempCane = deep_copy(ppew->tempCane);
		ppew->tempCaneDirty = false;
		ppew->tempCaneMutex.unlock();	
	
		// now lock the geometry
		ppew->tempGeometryMutex.lock();
		completed = generateMesh(myTempCane, &(ppew->tempGeometry), GlobalGraphicsSetting::VERY_HIGH);
		ppew->tempGeometryMutex.unlock();	
		ppew->geometryDirtyMutex.lock();
		ppew->geometryDirty = true;
		ppew->geometryDirtyMutex.unlock();

		ppew->tempCaneMutex.lock();
		caneChanged = ppew->tempCaneDirty;
		ppew->tempCaneMutex.unlock();
		if (caneChanged)
		{
			deep_delete(myTempCane);
			goto compute;
		}
		
		deep_delete(myTempCane);
		emit finishedMesh(completed, GlobalGraphicsSetting::VERY_HIGH);
	}
}



