
#include "pulltemplate.h"

PullTemplate :: PullTemplate()
{
	subpulls.clear();
}

void PullTemplate :: addSubpullTemplate(SubpullTemplate pull)
{
	subpulls.push_back(pull);
} 


