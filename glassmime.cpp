
#include "glassmime.h"

void encodeMimeData(char* mimeData, void* ptr, int type)
{
        sprintf(mimeData, "%p %d", ptr, type);
}

void decodeMimeData(const char* mimeData, void** ptr, int* type)
{
	sscanf(mimeData, "%p %d", ptr, type);
}

