
#include <QFile>
#include <QIODevice>
#include "glasscolor.h"
#include "cane.h"
#include "canetemplate.h"
#include "pickuptemplate.h"
#include "subcanetemplate.h"
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
	Cane* cane;
	if (qrand() % 2)
		cane = new Cane(CaneTemplate::BASE_CIRCLE);
	else
		cane = new Cane(CaneTemplate::BASE_SQUARE);

	cane->addCasing(outermostCasingShape);
	cane->setCasingColor(color, 0);
	cane->setCasingThickness((qrand() % 25) * 0.01 + 0.25, 0);

	return cane;
}

Cane* randomComplexCane(Cane* circleSimpleCane, Cane* squareSimpleCane)
{
	// set template
	// select a random template that is `complex', and is dependent upon the templates available
	// at revision 785 these are the templates between HORIZONTAL_LINE_CIRCLE and SURROUNDING_SQUARE
	int randomTemplateNumber = qrand() % (CaneTemplate::SURROUNDING_SQUARE - CaneTemplate::HORIZONTAL_LINE_CIRCLE) 
		+ CaneTemplate::HORIZONTAL_LINE_CIRCLE;
	Cane* cane = new Cane(static_cast<CaneTemplate::Type>(randomTemplateNumber));
	
	// set parameters
	cane->setTwist(0.0);
	cane->setCount(qrand() % 10 + 2);

	// set subcanes
	for (unsigned int i = 0; i < cane->subpullCount(); ++i)
	{
		SubcaneTemplate t = cane->subcaneTemplate(i);
		switch (t.shape)
		{
			case CIRCLE_SHAPE:
				t.cane = circleSimpleCane;
				break;
			case SQUARE_SHAPE:
				t.cane = squareSimpleCane;
				break;
		}
		cane->setSubcaneTemplate(t, i);
	}

	return cane;
}

Piece* randomPiece(Cane* cane1, Cane* cane2)
{
	int randomPickupTemplateNumber = qrand() % (PickupTemplate::lastSeedTemplate() - PickupTemplate::firstSeedTemplate())
		+ PickupTemplate::firstSeedTemplate();
	int randomPieceTemplateNumber = qrand() % (PieceTemplate::lastSeedTemplate() - PieceTemplate::firstSeedTemplate())
		+ PieceTemplate::firstSeedTemplate();
	Piece* piece = new Piece(static_cast<PieceTemplate::Type>(randomPieceTemplateNumber), static_cast<PickupTemplate::Type>(randomPickupTemplateNumber));

	// set parameters
	TemplateParameter p;
	for (unsigned int i = 0; i < piece->pickupParameterCount(); ++i)
	{
		piece->pickupParameter(i, &p);
		// not setting to upper intervals of parameter values for efficiency reasons 
		piece->setPickupParameter(i, qrand() % ((p.upperLimit - p.lowerLimit)/3) + p.lowerLimit);
	}	

	// set subcanes
	for (unsigned int i = 0; i < piece->subpickupCount(); ++i)
	{
		SubpickupTemplate t = piece->subpickupTemplate(i);
		t.cane = cane1;
		piece->setSubpickupTemplate(t, i);
	}
	// if a second cane is provided, alternate with the first one
	if (cane2 != NULL)
	{
		for (unsigned int i = 0; i < piece->subpickupCount(); i+=2)
		{
			SubpickupTemplate t = piece->subpickupTemplate(i);
			t.cane = cane2;
			piece->setSubpickupTemplate(t, i);
		}
	}

	return piece;
}


