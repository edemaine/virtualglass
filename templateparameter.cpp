
#include "templateparameter.h"

TemplateParameter :: TemplateParameter()
{
	value = 0;
	name = string("Unnamed");
	lowerLimit = upperLimit = 0;
}

TemplateParameter :: TemplateParameter(int _value, string _name, int _lowerLimit, int _upperLimit)
{
	value = _value;
	name = _name;
	lowerLimit = _lowerLimit;
	upperLimit = _upperLimit;
}




