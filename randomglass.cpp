

#include <QtGlobal>
#include <QFile>
#include <QIODevice>
#include "glasscolor.h"
#include "pullplan.h"
#include "pulltemplate.h"
#include "pickupplan.h"
#include "pickuptemplate.h"
#include "subpulltemplate.h"
#include "piece.h"
#include "piecetemplate.h"
#include "colorreader.h"
#include "randomglass.h"

GlassColor* randomGlassColor()
{
	// Sample from reichenbach transparents
        QFile file(":/reichenbach-transparent-colors.vgc");

	// First see how many colors are in there
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	unsigned int totalColors = 0;
	while (!file.atEnd())
	{
		QString line = file.readLine();
		line = line.trimmed();
		if (line.isEmpty())
			continue;

		if (line.at(0) == '[')
			++totalColors;
	}

	unsigned int colorChoice = qrand() % totalColors;
	file.seek(0);	
	Color color;
	QString colorName;
	unsigned int count = 0;
	while (!file.atEnd())
	{
		QString line = file.readLine();
		line = line.trimmed();
		if (line.isEmpty())
			continue;

		if (line.at(0) == '[')
		{
			if (count == colorChoice)
			{
				colorName = lineToColorName(line);
				color = lineToColorRGB(file.readLine());	
				break;
			}
			++count;
		}
	}
	file.close();

	return new GlassColor(color, shortColorName(colorName));
}

PullPlan* randomSimplePullPlan(enum GeometricShape outermostCasingShape, GlassColor* color)
{
	PullPlan* plan;
	if (qrand() % 2)
		plan = new PullPlan(PullTemplate::BASE_CIRCLE);
	else
		plan = new PullPlan(PullTemplate::BASE_SQUARE);

	plan->setCasingColor(color, 0);
	plan->setCasingThickness((qrand() % 25) * 0.01 + 0.25, 0);
	plan->setOutermostCasingShape(outermostCasingShape);

	return plan;
}

PullPlan* randomComplexPullPlan(PullPlan* circleSimplePlan, PullPlan* squareSimplePlan)
{
	// set template
	// select a random template that is `complex', and is dependent upon the templates available
	// at revision 785 these are the templates between HORIZONTAL_LINE_CIRCLE and SURROUNDING_SQUARE
	int randomTemplateNumber = qrand() % (PullTemplate::SURROUNDING_SQUARE - PullTemplate::HORIZONTAL_LINE_CIRCLE) 
		+ PullTemplate::HORIZONTAL_LINE_CIRCLE;
	PullPlan* plan = new PullPlan(static_cast<PullTemplate::Type>(randomTemplateNumber));
	
	// set parameters
	plan->setTwist(qrand() % 50 - 25);
	TemplateParameter p;
	for (unsigned int i = 0; i < plan->getParameterCount(); ++i)
	{
		plan->getParameter(i, &p);
		plan->setParameter(i, qrand() % ((p.upperLimit - p.lowerLimit)/2) + p.lowerLimit);
	}	

	// set subplans
	plan->getParameter(0, &p);	
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

PickupPlan* randomPickup(PullPlan* pullPlan1, PullPlan* pullPlan2)
{
        int randomTemplateNumber = qrand() % (PickupTemplate::lastSeedTemplate() - PickupTemplate::firstSeedTemplate())
                + PickupTemplate::firstSeedTemplate();
	PickupPlan* pickup = new PickupPlan(static_cast<PickupTemplate::Type>(randomTemplateNumber));

	// set parameters
	TemplateParameter p;
	for (unsigned int i = 0; i < pickup->getParameterCount(); ++i)
	{
		pickup->getParameter(i, &p);
		// not setting to upper intervals of parameter values for efficiency reasons 
		pickup->setParameter(i, qrand() % ((p.upperLimit - p.lowerLimit)/3) + p.lowerLimit);
	}	

	// set subplans
	for (unsigned int i = 0; i < pickup->subs.size(); ++i)
	{
		pickup->subs[i].plan = pullPlan1;
	}
	// if a second plan is provided, alternate with the first one
	if (pullPlan2 != NULL)
	{
		for (unsigned int i = 0; i < pickup->subs.size(); i+=2)
		{
			pickup->subs[i].plan = pullPlan2;
		}
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


