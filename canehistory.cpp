
#include "canehistory.h"

CaneHistory :: CaneHistory()
{
	past = new Cane*[100];
	future = new Cane*[100];
	maxSize = 100;
	curPast = curFuture = 0;
}

void CaneHistory :: saveState(Cane* c)
{
	if (curPast == maxSize)
		doubleSize();

	// If you have `undone' some things
	// then saving the current state implies
	// throwing away the other branch of the future
	if (curFuture != 0)
		curFuture = 0;

	curPast++;
	past[curPast] = c;
}

Cane* CaneHistory :: getState()
{
	return past[curPast];
}

void CaneHistory :: undo()
{
	if (curPast == 0)
		return;
	curFuture++;
	future[curFuture] = past[curPast];
	curPast--;
}

// Returns whether there's a future to roll forward into
bool CaneHistory :: canRedo()
{
	return (curFuture > 0);
}

void CaneHistory :: redo()
{
	if (curFuture == 0)
		return;

	curPast++;
	past[curPast] = future[curFuture];
	curFuture--;
}


void CaneHistory :: doubleSize()
{
	Cane** newPast = new Cane*[2*maxSize];
	Cane** newFuture = new Cane*[2*maxSize];
	for (int i = 0; i < maxSize; ++i)
	{
		newPast[i] = past[i];
		newFuture[i] = future[i];
	}
	maxSize *= 2;
	delete[] past;
	delete[] future;
	past = newPast;
	future = newFuture;
}



