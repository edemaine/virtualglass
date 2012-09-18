
#ifndef PULLTEMPLATE_H
#define PULLTEMPLATE_H

// Pull template types
namespace PullTemplate
{
        // When adding a new template, be sure to update the names[] array below
        // and the first/last template functions in pulltemplate.cpp
	enum Type
	{
		baseCircle=1,
		baseSquare,
		horizontalLineCircle, 
		horizontalLineSquare,
		tripod,
		cross,
		squareOfCircles,
		squareOfSquares,
		surroundingCircle,
		surroundingSquare,
		customCircle,
		customSquare
	};

	enum Type firstTemplate();
	enum Type lastTemplate();
	enum Type firstSeedTemplate();
	enum Type lastSeedTemplate();

	const char* enumToString(enum Type t);
}

#endif

