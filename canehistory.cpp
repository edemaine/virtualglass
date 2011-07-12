
#include "canehistory.h"

CaneHistory :: CaneHistory()
{
	past = new Cane*[100];
	maxSize = 100;
	curPast = 0;
}

void CaneHistory :: saveState(Cane* c)
{
	if (curPast == maxSize)
		doubleSize();

	if (c != NULL)
		past[curPast] = c->deepCopy();
	curPast++;
}

Cane* CaneHistory :: getState()
{
	if (curPast < 1)
		return NULL;
	return past[curPast-1];
}

void CaneHistory :: undo()
{
	if (curPast == 0)
		return;
	curPast--;
}

void CaneHistory :: doubleSize()
{
	Cane** newPast = new Cane*[2*maxSize];
	for (int i = 0; i < maxSize; ++i)
	{
		newPast[i] = past[i];
	}
	maxSize *= 2;
	delete[] past;
	past = newPast;
}

bool CaneHistory :: isInitial()
{
	return curPast<=1;
}



