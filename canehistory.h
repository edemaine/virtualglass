
#ifndef CANEHISTORY_H
#define CANEHISTORY_H

#include "cane.h"

class CaneHistory
{
	public:
		CaneHistory();
		void saveState(Cane* c);
		Cane* getState();
		void undo();
		void redo();

	private:
		Cane** past;
		Cane** future;
		int maxSize;
		int curPast;
		int curFuture;
		void doubleSize();	
};

#endif

