
#include "canehistory.h"

CaneHistory :: CaneHistory()
{
	buffer[0] = NULL;
	curPosition = 0; // The current last saved state
	endOfValid = 0; // The end of saved states
}

void CaneHistory :: saveState(Cane* c)
{
	// If you're out of space, just scoot everything backwards
	if (curPosition == 19)
	{
		for (int i = 1; i < 20; i++)
			buffer[i-1] = buffer[i];
		buffer[19] = c;
	}
	// Otherwise move forward and save the state
	else
	{
		++curPosition;
		endOfValid = curPosition;
		buffer[curPosition] = c;
	}
}

Cane* CaneHistory :: getState()
{
	return buffer[curPosition];
}

bool CaneHistory :: canUndo()
{
	return (curPosition > 0);
}

Cane* CaneHistory :: undo()
{
	if (canUndo())
	{
		--curPosition;
		return getState();
	}
	else
		return getState();
}

bool CaneHistory :: canRedo()
{
	return !(curPosition == 19 || curPosition == endOfValid);
}

Cane* CaneHistory :: redo()
{
	if (canRedo())
	{
		++curPosition;
		return buffer[curPosition]; 
	}
	else
		return buffer[curPosition];
}



