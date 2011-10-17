
#ifndef PULLTEMPLATE_H
#define PULLTEMPLATE_H

#include <vector>
#include "primitives.h"
#include "subpulltemplate.h"

using std::vector;

class PullTemplate
{
	public:
		PullTemplate();
		void addSubpullTemplate(SubpullTemplate sub);
		vector<SubpullTemplate> subpulls;
};

#endif

