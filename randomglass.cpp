
#include <QFile>
#include <QIODevice>
#include "glasscolor.h"
#include "cane.h"
#include "pulltemplate.h"
#include "pickup.h"
#include "pickuptemplate.h"
#include "subpulltemplate.h"
#include "piece.h"
#include "piecetemplate.h"
#include "randomglass.h"
#include "glassfileio.h"
#include "globalglass.h"

GlassColor* randomGlassColor()
{
	vector<GlassColor*> colors;
	QString collectionName;

	if(!readColorFile(":/vgc/reichenbach-opaque-colors.vgc", collectionName, colors))
		return GlobalGlass::color();
	
	unsigned int choice = qrand() % colors.size();
	for (unsigned int i = 0; i < colors.size(); ++i)
	{
		if (i != choice)
			delete colors[i];
	}

	return colors[choice];
}

Cane* randomSimpleCane(enum GeometricShape outermostCasingShape, GlassColor* color)
{
	Cane* plan;
	if (qrand() % 2)
		plan = new Cane(PullTemplate::BASE_CIRCLE);
	else
		plan = new Cane(PullTemplate::BASE_SQUARE);

	plan->addCasing(outermostCasingShape);
	plan->setCasingColor(color, 0);
	plan->setCasingThickness((qrand() % 25) * 0.01 + 0.25, 0);

	return plan;
}

Cane* randomComplexCane(Cane* circleSimplePlan, Cane* squareSimplePlan)
{
	// set template
	// select a random template that is `complex', and is dependent upon the templates available
	// at revision 785 these are the templates between HORIZONTAL_LINE_CIRCLE and SURROUNDING_SQUARE
	int randomTemplateNumber = qrand() % (PullTemplate::SURROUNDING_SQUARE - PullTemplate::HORIZONTAL_LINE_CIRCLE) 
		+ PullTemplate::HORIZONTAL_LINE_CIRCLE;
	Cane* plan = new Cane(static_cast<PullTemplate::Type>(randomTemplateNumber));
	
	// set parameters
	plan->setTwist(0.0);
	plan->setCount(qrand() % 10 + 2);

	// set subplans
	for (unsigned int i = 0; i < plan->subpullCount(); ++i)
	{
		SubpullTemplate t = plan->getSubpullTemplate(i);
		switch (t.shape)
		{
			case CIRCLE_SHAPE:
				t.plan = circleSimplePlan;
				break;
			case SQUARE_SHAPE:
				t.plan = squareSimplePlan;
				break;
		}
		plan->setSubpullTemplate(t, i);
	}

	return plan;
}

Pickup* randomPickup(Cane* pullPlan1, Cane* pullPlan2)
{
	int randomTemplateNumber = qrand() % (PickupTemplate::lastSeedTemplate() - PickupTemplate::firstSeedTemplate())
		+ PickupTemplate::firstSeedTemplate();
	Pickup* pickup = new Pickup(static_cast<PickupTemplate::Type>(randomTemplateNumber));

	// set parameters
	TemplateParameter p;
	for (unsigned int i = 0; i < pickup->parameterCount(); ++i)
	{
		pickup->getParameter(i, &p);
		// not setting to upper intervals of parameter values for efficiency reasons 
		pickup->setParameter(i, qrand() % ((p.upperLimit - p.lowerLimit)/3) + p.lowerLimit);
	}	

	// set subplans
	for (unsigned int i = 0; i < pickup->subpickupCount(); ++i)
	{
		SubpickupTemplate t = pickup->getSubpickupTemplate(i);
		t.plan = pullPlan1;
		pickup->setSubpickupTemplate(t, i);
	}
	// if a second plan is provided, alternate with the first one
	if (pullPlan2 != NULL)
	{
		for (unsigned int i = 0; i < pickup->subpickupCount(); i+=2)
		{
			SubpickupTemplate t = pickup->getSubpickupTemplate(i);
			t.plan = pullPlan2;
			pickup->setSubpickupTemplate(t, i);
		}
	}


	return pickup;
}

Piece* randomPiece(Pickup* pickup)
{
	int randomTemplateNumber = qrand() % (PieceTemplate::lastSeedTemplate() - PieceTemplate::firstSeedTemplate())
		+ PieceTemplate::firstSeedTemplate();
	Piece* piece = new Piece(static_cast<PieceTemplate::Type>(randomTemplateNumber));

	// set pickup
	piece->setPickup(pickup);

	return piece;
}


