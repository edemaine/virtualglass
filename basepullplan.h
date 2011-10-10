

#ifndef BASEPULLPLAN_H
#define BASEPULLPLAN_H

#include <stdlib.h>
#include <vector>
#include "primitives.h"

class BasePullPlan
{
	public:
		BasePullPlan(Color c);
		Color getColor();
		void setColor(Color c);
	
	private:
		Color color;	
};

#endif

