
#include "piecetemplate.h"
#include <stdio.h>

PieceTemplate :: PieceTemplate(int t)
{
	char* tmp;

	this->type = t;

	switch (this->type)
	{
		case TUMBLER_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Width");
			parameterNames.push_back(tmp);
			tmp = new char[100];
			sprintf(tmp, "Taper");
			parameterNames.push_back(tmp);
			parameterValues.push_back(0);
			parameterValues.push_back(0);
			break;
		case BOWL_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Shallowness");
			parameterNames.push_back(tmp);
			tmp = new char[100];
			sprintf(tmp, "Twist");
			parameterNames.push_back(tmp);
			parameterValues.push_back(0);
			parameterValues.push_back(0);
			break;
	}
}




