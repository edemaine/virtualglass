
#include "templateparameter.h"

TemplateParameter :: TemplateParameter()
{
	this->value = 0;
	this->name = string("Unnamed");
}

TemplateParameter :: TemplateParameter(int _value, string _name)
{
	this->value = _value;
	this->name = _name;
}


