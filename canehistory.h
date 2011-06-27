
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

	private:
		Cane** past;
		int maxSize;
		int curPast;
		void doubleSize();
};

#endif

