
#ifndef PULLTEMPLATE_H
#define PULLTEMPLATE_H

#include <vector>
#include "primitives.h"
#include "subpulltemplate.h"
#include "constants.h"

using std::vector;

class PullTemplate
{
	public:
		PullTemplate(int t);
		vector<SubpullTemplate> subpulls;
		int shape;	
		int type;
};

#endif

