
#ifndef GLOBALBACKGROUNDCOLOR_H
#define GLOBALBACKGROUNDCOLOR_H

#include <QColor>
#include <vector>
#include "primitives.h"

namespace GlobalBackgroundColor
{
	// fval MUST be set to ival / 255.0 
	const int ival = 200;
	const QColor qcolor = QColor(ival, ival, ival, 255);
	const float fval = 0.78431372549019607f;
	const Color color = make_vector<float>(fval, fval, fval, 1.0f);
}

#endif


