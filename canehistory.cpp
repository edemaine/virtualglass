
#include "canehistory.h"

CaneHistory :: CaneHistory()
{
	past = new Cane*[100];
	for (int i=0;i<100;i++)
	{
		past[i]=NULL;
	}
	maxSize = 100;
	curPast = 0; //everything from curPast-1 and less is a saved state, 0 represents currently viewed state
	maxCur = -1; //represents the most advanced the history is in its current timeline
	isChanging = false;
}

void CaneHistory :: clearRecentState(int index)
{
	for(int i=index+1;i<=maxCur;i++)
	{
		past[i]=NULL;
	}
	maxCur = index-1;
}

void CaneHistory :: saveState(Cane* c)
{
	if (curPast == maxSize)
		doubleSize();

	if (c != NULL)
		past[curPast] = c->deepCopy();
	curPast++;
	if (maxCur+1<curPast)
		maxCur++;
	else
		clearRecentState(curPast);
}

Cane* CaneHistory :: getState()
{
	if (curPast < 0 || curPast>maxCur+1)
		return NULL;
	return past[curPast];
}

Cane* CaneHistory :: undo()
{
	if (curPast == 0)
		return getState();
	curPast--;
	return getState();
}

Cane* CaneHistory :: redo()
{
	if (curPast > maxCur + 1)
		return getState();
	curPast++;
	return getState();
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

bool CaneHistory :: isAvailable()
{
	return !isChanging;
}

bool CaneHistory :: isBusy()
{
	return isChanging;
}

void CaneHistory :: setBusy(bool isChanging)
{
	this->isChanging = isChanging;
}
bool CaneHistory :: isMostRecent()
{
	return curPast == maxCur + 1;
}
