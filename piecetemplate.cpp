
#include "piecetemplate.h"
#include <stdio.h>

PieceTemplate :: PieceTemplate(int t)
{
	char* tmp;

	this->type = t;

	switch (this->type)
	{
		case VASE_PIECE_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Body width");
			parameterNames.push_back(tmp);
			tmp = new char[100];
			sprintf(tmp, "Lip width");
			parameterNames.push_back(tmp);
			parameterValues.push_back(0);
			parameterValues.push_back(0);
			break;
		case TUMBLER_PIECE_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Size");
			parameterNames.push_back(tmp);
			tmp = new char[100];
			sprintf(tmp, "Roundedness");
			parameterNames.push_back(tmp);
			parameterValues.push_back(0);
			parameterValues.push_back(0);
			break;
		case BOWL_PIECE_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Radius");
			parameterNames.push_back(tmp);
			tmp = new char[100];
			sprintf(tmp, "Twist");
			parameterNames.push_back(tmp);
			parameterValues.push_back(0);
			parameterValues.push_back(0);
			break;
		case POT_PIECE_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Body radius");
			parameterNames.push_back(tmp);
			tmp = new char[100];
			sprintf(tmp, "Lip radius");
			parameterNames.push_back(tmp);
			parameterValues.push_back(0);
			parameterValues.push_back(0);
			break;
		case WAVY_PLATE_PIECE_TEMPLATE:
			tmp = new char[100];
			sprintf(tmp, "Wave count");
			parameterNames.push_back(tmp);
			tmp = new char[100];
			sprintf(tmp, "Wave depth");
			parameterNames.push_back(tmp);
			parameterValues.push_back(0);
			parameterValues.push_back(0);
			break;
	}
}

PieceTemplate* PieceTemplate :: copy()
{
	PieceTemplate* c = new PieceTemplate(this->type);

	c->parameterValues.clear();
        for (unsigned int i = 0; i < this->parameterValues.size(); ++i)
        {
                c->parameterValues.push_back(this->parameterValues[i]);
        }

	return c;
}


