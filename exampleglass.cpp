
#include "exampleglass.h"

void web1Piece(GlassColor** gc, PullPlan** pp1, PullPlan** pp2, Piece** p)
{
	Color c;
	c.r = 20/255.0;
	c.g = 0.0;
	c.b = 180/255.0;
	c.a = 0.3;
	*gc = new GlassColor(c, "");

	*pp1 = new PullPlan(PullTemplate::BASE_SQUARE);	
	(*pp1)->setOutermostCasingShape(SQUARE_SHAPE);
	(*pp1)->setCasingColor(*gc, 0);
	(*pp1)->setCasingThickness(0.5, 0);

	*pp2 = new PullPlan(PullTemplate::BASE_SQUARE);	
	(*pp2)->setOutermostCasingShape(SQUARE_SHAPE);
	(*pp2)->setCasingColor(*gc, 1);
	(*pp2)->setCasingThickness(0.5, 0);

	*p = new Piece(PieceTemplate::VASE);
	(*p)->pickup->setTemplateType(PickupTemplate::VERTICAL);
	(*p)->pickup->setParameter(0, 20);	
	for (unsigned int i = 0; i < (*p)->pickup->subs.size(); i += 2)
		(*p)->pickup->subs[i].plan = (*pp1);
	for (unsigned int i = 1; i < (*p)->pickup->subs.size(); i += 2)
		(*p)->pickup->subs[i].plan = (*pp2);
	(*p)->setParameter(0, 15);
	(*p)->setParameter(1, 65);
	(*p)->setParameter(2, 0);
}


