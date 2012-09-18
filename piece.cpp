
#include "piece.h"

Piece :: Piece(int templateType)
{
	setTemplateType(templateType);
	// initialize the piece's pickup to be something boring and base
	this->pickup = new PickupPlan(PickupTemplate::verticals);
}

bool Piece :: hasDependencyOn(PullPlan* plan)
{
        bool pickupPlansDependOn = false;

        for (unsigned int i = 0; i < this->pickup->subs.size(); ++i)
        {
                if (this->pickup->subs[i].plan->hasDependencyOn(plan))
                {
                        pickupPlansDependOn = true;
                        break;
                }
        }

        return pickupPlansDependOn;
}


bool Piece :: hasDependencyOn(GlassColor* glassColor)
{
	bool pickupPlansDependOn = false;

	for (unsigned int i = 0; i < pickup->subs.size(); ++i)
	{
		if (pickup->subs[i].plan->hasDependencyOn(glassColor))
		{
			pickupPlansDependOn = true;		
			break;
		}
	}
	
	if (pickup->overlayGlassColor == glassColor || pickup->underlayGlassColor == glassColor)
		pickupPlansDependOn = true;

	return pickupPlansDependOn;
}

/*
copy() is intended to be a copy at the correct depth consistent with
a Piece as represented in the GUI: it is a shape and a pickup plan, 
but does not include the pull plans used.
*/
Piece* Piece :: copy() const
{
	Piece* c = new Piece(this->templateType);

	c->templateType = this->templateType;
        for (unsigned int i = 0; i < this->parameterNames.size(); ++i)
        {
                c->parameterValues[i] = this->parameterValues[i];
        }

	c->pickup = this->pickup->copy();
	
	return c;
}

unsigned int Piece :: getParameterCount()
{
	return parameterNames.size();
}

char* Piece :: getParameterName(unsigned int index)
{
	// right now this is a total hack...we just pass pointers
	// along, but they can be modified by the caller
	assert(index < parameterNames.size());
	return parameterNames[index];
}

int Piece :: getParameter(unsigned int index)
{
	assert(index < parameterValues.size());
	return parameterValues[index];
}

void Piece :: setParameter(unsigned int index, int value)
{
	assert(index < parameterValues.size());
	parameterValues[index] = value;
}

void Piece :: setTemplateType(int _templateType, bool force)
{
        if (!force && this->templateType == _templateType)
                return;

	this->templateType = _templateType;

	parameterNames.clear();
        parameterValues.clear();
        char* tmp;
        switch (this->templateType)
        {
                case VASE_PIECE_TEMPLATE:
                        tmp = new char[100];
                        sprintf(tmp, "Lip width");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(0);
                        tmp = new char[100];
                        sprintf(tmp, "Body width");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(0);
                        tmp = new char[100];
                        sprintf(tmp, "Twist");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(0);
                        break;
                case TUMBLER_PIECE_TEMPLATE:
                        tmp = new char[100];
                        sprintf(tmp, "Size");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(0);
                        tmp = new char[100];
                        sprintf(tmp, "Roundedness");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(0);
                        break;
                case BOWL_PIECE_TEMPLATE:
                        tmp = new char[100];
                        sprintf(tmp, "Openness");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(0);
                        tmp = new char[100];
                        sprintf(tmp, "Size");
                        parameterNames.push_back(tmp);
                        parameterValues.push_back(0);
                        tmp = new char[100];
                        sprintf(tmp, "Twist");
                        parameterNames.push_back(tmp);
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

int Piece :: getTemplateType()
{
	return this->templateType;
}

Piece *deep_copy(const Piece *_piece) {
	assert(_piece);
	Piece *piece = _piece->copy();
	//Replace piece with a deep copy:
	delete piece->pickup;
	piece->pickup = deep_copy(_piece->pickup);
	return piece;
}

void deep_delete(Piece *piece) {
	assert(piece);
	deep_delete(piece->pickup);
	piece->pickup = NULL;
	delete piece;
}
