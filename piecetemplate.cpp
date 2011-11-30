
#include "piecetemplate.h"
#include <stdio.h>

PieceTemplate :: PieceTemplate(int t)
{
	char* tmp;

	this->type = t;

	switch (this->type)
	{
		case VASE_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Body width");
			parameterNames.push_back(tmp);
			tmp = new char[100];
			sprintf(tmp, "Lip width");
			parameterNames.push_back(tmp);
			parameterValues.push_back(0);
			parameterValues.push_back(0);
			break;
		case TUMBLER_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Size");
			parameterNames.push_back(tmp);
			tmp = new char[100];
			sprintf(tmp, "Roundedness");
			parameterNames.push_back(tmp);
			parameterValues.push_back(0);
			parameterValues.push_back(0);
			break;
		case BOWL_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Radius");
			parameterNames.push_back(tmp);
			tmp = new char[100];
			sprintf(tmp, "Twist");
			parameterNames.push_back(tmp);
			parameterValues.push_back(0);
			parameterValues.push_back(0);
			break;
	}
}




