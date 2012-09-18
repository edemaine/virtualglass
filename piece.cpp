
#include "piece.h"

Piece :: Piece(enum PieceTemplate::Type _type)
{
	setTemplateType(_type);
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
	Piece* c = new Piece(type);

        for (unsigned int i = 0; i < parameters.size(); ++i)
        {
                c->parameters[i].value = parameters[i].value;
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
	dest->value = parameters[_index].value;
	dest->name = parameters[_index].name; // this is a copy, since it's a std::string
}

void Piece :: setParameter(unsigned int _index, int _value)
{
	assert(_index < parameters.size());
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
                case PieceTemplate::vase:
			parameters.push_back(TemplateParameter(0, string("Lip width")));
			parameters.push_back(TemplateParameter(0, string("Body width")));
			parameters.push_back(TemplateParameter(0, string("Twist")));
                        break;
                case PieceTemplate::tumbler:
			parameters.push_back(TemplateParameter(0, string("Size")));
			parameters.push_back(TemplateParameter(0, string("Roundedness")));
                        break;
                case PieceTemplate::bowl:
			parameters.push_back(TemplateParameter(0, string("Openness")));
			parameters.push_back(TemplateParameter(0, string("Size")));
			parameters.push_back(TemplateParameter(0, string("Twist")));
                        break;
                case PieceTemplate::pot:
			parameters.push_back(TemplateParameter(0, string("Body radius")));
			parameters.push_back(TemplateParameter(0, string("Lip radius")));
                        break;
                case PieceTemplate::wavyPlate:
			parameters.push_back(TemplateParameter(0, string("Wave count")));
			parameters.push_back(TemplateParameter(0, string("Wave depth")));
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
