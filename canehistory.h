
#ifndef CANEHISTORY_H
#define CANEHISTORY_H

#include "cane.h"

class CaneHistory
{
public:
	CaneHistory();
	void saveState(Cane* c);
	Cane* getState();
	bool canUndo();
	Cane* undo();
	bool canRedo();
	Cane* redo();
	int getPosition();

private:
	Cane* buffer[20];
	int curPosition;
	int endOfValid;
};

#endif

