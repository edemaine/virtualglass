
#include "globaldepthpeelingsetting.h"

namespace GlobalDepthPeelingSetting
{
	bool peel = true;

	bool enabled()
	{
		return peel;
	}

	void setEnabled(bool enable)
	{
		peel = enable;
	}
}

