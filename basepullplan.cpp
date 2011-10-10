

#include "basepullplan.h"

BasePullPlan :: BasePullPlan(Color c)
{
	this->color = c;
}

Color BasePullPlan :: getColor()
{
	return this->color;
}

void BasePullPlan :: setColor(Color c)
{
	this->color = c;
} 


