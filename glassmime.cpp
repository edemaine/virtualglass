
#include "glassmime.h"

void encodeMimeData(char* mimeData, PullPlan* plan, int type)
{
        sprintf(mimeData, "%p %d", plan, type);
}

void decodeMimeData(const char* mimeData, PullPlan** plan, int* type)
{
	sscanf(mimeData, "%p %d", plan, type);
}

