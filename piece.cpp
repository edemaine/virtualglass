
#include "piece.h"

Piece :: Piece(enum PieceTemplate::Type _type)
{
	setTemplateType(_type);
	// initialize the piece's pickup to be something boring and base
	this->pickup = new PickupPlan(PickupTemplate::VERTICAL);
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
	Piece* c = new Piece(type);

        for (unsigned int i = 0; i < parameters.size(); ++i)
        {
                c->parameters[i] = parameters[i];
        }

	c->pickup = this->pickup->copy();
	
	return c;
}

unsigned int Piece :: getParameterCount()
{
	return parameters.size();
}

void Piece :: getParameter(unsigned int _index, TemplateParameter* dest)
{
	assert(_index < parameters.size());
	*dest = parameters[_index];
}

void Piece :: setParameter(unsigned int _index, int _value)
{
	assert(_index < parameters.size());
	assert(parameters[_index].lowerLimit <= _value <= parameters[_index].upperLimit);
	parameters[_index].value = _value;
}

void Piece :: setTemplateType(enum PieceTemplate::Type _type, bool force)
{
        if (!force && this->type == _type)
                return;

	this->type = _type;

	parameters.clear();
        switch (type)
        {
		// Is it possible to just make a string const and hope it's 
		// kept around as long as the TemplateParameter is?
                case PieceTemplate::VASE:
			parameters.push_back(TemplateParameter(0, string("Lip width"), 0, 100));
			parameters.push_back(TemplateParameter(0, string("Body width"), 0, 100));
			parameters.push_back(TemplateParameter(0, string("Twist"), 0, 100));
                        break;
                case PieceTemplate::TUMBLER:
			parameters.push_back(TemplateParameter(30, string("Width"), 0, 100));
			parameters.push_back(TemplateParameter(50, string("Roundedness"), 0, 100));
			parameters.push_back(TemplateParameter(0, string("Twist"), 0, 100));
                        break;
                case PieceTemplate::BOWL:
			parameters.push_back(TemplateParameter(10, string("Openness"), 0, 100));
			parameters.push_back(TemplateParameter(0, string("Size"), 0, 100));
			parameters.push_back(TemplateParameter(0, string("Twist"), 0, 100));
                        break;
                case PieceTemplate::POT:
			parameters.push_back(TemplateParameter(50, string("Lip width"), 0, 100));
			parameters.push_back(TemplateParameter(50, string("Body width"), 0, 100));
			parameters.push_back(TemplateParameter(50, string("Bottom width"), 0, 100));
                        break;
                case PieceTemplate::WAVY_PLATE:
			parameters.push_back(TemplateParameter(30, string("Wave count"), 0, 100));
			parameters.push_back(TemplateParameter(50, string("Wave depth"), 0, 100));
                        break;
                case PieceTemplate::PICKUP:
			break;
        }

}

enum PieceTemplate::Type Piece :: getTemplateType()
{
	return type;
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
