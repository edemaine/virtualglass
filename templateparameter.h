
#ifndef TEMPLATEPARAMETER_H
#define TEMPLATEPARAMETER_H

#include <string>

using namespace std;

class TemplateParameter
{
	public:
		TemplateParameter();
		TemplateParameter(int value, string name, int lowerLimit, int upperLimit);
		int value;
		string name;
		int lowerLimit;
		int upperLimit;
};

#endif
