
#ifndef PULLTEMPLATE_H
#define PULLTEMPLATE_H

// Pull template types
namespace PullTemplate
{
	enum Type
	{
		baseCircle,
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

	enum Type firstPullTemplate();
	enum Type lastPullTemplate();

/*
	const char* pullTemplateNames[] = 
	{
		"Base Circle",
		"Base Square",
		"Horizontal Line Circle",
		"Horizontal Line Square",
		"Tripod",
		"Cross",
		"Square of Circles",
		"Square of Squares",
		"Surrounding Circle",
		"Surrounding Square",
		"Custom Circle",
		"Custom Square"
	};
*/
}

#endif

