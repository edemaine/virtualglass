
#include "randomglass.h"

GlassColor* randomGlassColor()
{
	Color c;
	c.a = 0.5;
	c.r = (qrand() % 256) / 255.0;
	c.g = (qrand() % 256) / 255.0;
	c.b = (qrand() % 256) / 255.0;

	return new GlassColor(c, "???");
}

PullPlan* randomSimplePullPlan(enum GeometricShape outermostCasingShape, GlassColor* color)
{
	PullPlan* plan;
	if (qrand() % 2)
	{
		plan = new PullPlan(PullTemplate::BASE_CIRCLE);
	}
	else
	{
		plan = new PullPlan(PullTemplate::BASE_SQUARE);
	} 	

	plan->setCasingColor(color, 0);
	plan->setCasingThickness((qrand() % 50) * 0.01 + 0.25, 0);
	plan->setOutermostCasingShape(outermostCasingShape);

	return plan;
}

PullPlan* randomComplexPullPlan(PullPlan* circleSimplePlan, PullPlan* squareSimplePlan)
{
	// set template
	int randomTemplateNumber = qrand() % (PullTemplate::lastSeedTemplate() - PullTemplate::firstSeedTemplate()) 
		+ PullTemplate::firstSeedTemplate();
	PullPlan* plan = new PullPlan(static_cast<PullTemplate::Type>(randomTemplateNumber));
	
	// set parameters
	TemplateParameter p;
	for (unsigned int i = 0; i < plan->getParameterCount(); ++i)
	{
		plan->getParameter(i, &p);
		plan->setParameter(i, qrand() % (p.upperLimit - p.lowerLimit) + p.lowerLimit);
	}	

	// set subplans
	for (unsigned int i = 0; i < plan->subs.size(); ++i)
	{
		switch (plan->subs[i].shape)
		{
			case CIRCLE_SHAPE:
				plan->subs[i].plan = circleSimplePlan;
				break;
			case SQUARE_SHAPE:
				plan->subs[i].plan = squareSimplePlan;
				break;
		}
	}

	return plan;
}

PickupPlan* randomPickupPlan(PullPlan* pullPlan)
{
        int randomTemplateNumber = qrand() % (PickupTemplate::lastSeedTemplate() - PickupTemplate::firstSeedTemplate())
                + PickupTemplate::firstSeedTemplate();
	PickupPlan* pickup = new PickupPlan(static_cast<PickupTemplate::Type>(randomTemplateNumber));

	// set parameters
	TemplateParameter p;
	for (unsigned int i = 0; i < pickup->getParameterCount(); ++i)
	{
		pickup->getParameter(i, &p);
		pickup->setParameter(i, qrand() % (p.upperLimit - p.lowerLimit) + p.lowerLimit);
	}	

	// set subplans
	for (unsigned int i = 0; i < pickup->subs.size(); ++i)
	{
		pickup->subs[i].plan = pullPlan;
	}

	return pickup;
}

Piece* randomPiece(PickupPlan* pickup)
{
        int randomTemplateNumber = qrand() % (PieceTemplate::lastSeedTemplate() - PieceTemplate::firstSeedTemplate())
                + PieceTemplate::firstSeedTemplate();
	Piece* piece = new Piece(static_cast<PieceTemplate::Type>(randomTemplateNumber));

	// set piece parameters
	TemplateParameter p;
	for (unsigned int i = 0; i < piece->getParameterCount(); ++i)
	{
		piece->getParameter(i, &p);
		piece->setParameter(i, qrand() % (p.upperLimit - p.lowerLimit) + p.lowerLimit);
	}	

	// set pickup
	piece->pickup = pickup;

	return piece;
}


