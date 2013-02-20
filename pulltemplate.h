
#ifndef PULLTEMPLATE_H
#define PULLTEMPLATE_H

// Pull template types
namespace PullTemplate
{
	// When adding a new template, be sure to update the names[] array below
	// and the first/last template functions in pulltemplate.cpp
	enum Type
	{
		BASE_CIRCLE=1,
		BASE_SQUARE,
		HORIZONTAL_LINE_CIRCLE,
		HORIZONTAL_LINE_SQUARE,
		TRIPOD,
		CROSS,
		SQUARE_OF_CIRCLES,
		SQUARE_OF_SQUARES,
		SURROUNDING_CIRCLE,
		SURROUNDING_SQUARE,
		CUSTOM,
	};

	enum Type firstSeedTemplate();
	enum Type lastSeedTemplate();
}

#endif

