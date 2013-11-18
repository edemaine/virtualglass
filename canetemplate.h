
#ifndef CANETEMPLATE_H
#define CANETEMPLATE_H

// Pull template types
namespace CaneTemplate
{
	// When adding a new template, be sure to update the names[] array below
	// and the first/last template functions in canetemplate.cpp
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

	bool templateHasNoSubcanes(CaneTemplate::Type t);
	bool templateHasSubcanes(CaneTemplate::Type t);
}

#endif

