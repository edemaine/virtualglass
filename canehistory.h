
#ifndef CANEHISTORY_H
#define CANEHISTORY_H

#include "cane.h"

class CaneHistory
{
public:
	CaneHistory();
	void saveState(Cane* c);
	Cane* getState();
	Cane* undo();
	Cane* redo();
	bool isMostRecent();
	bool isAvailable();
	bool isBusy();
	void setBusy(bool isChanging);

private:
	Cane** past;
	int maxSize;
	int curPast;
	int maxCur;
	void doubleSize();
	void clearRecentState(int index);
	bool isChanging;
};

#endif

