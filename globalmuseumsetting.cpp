
#include "globalmuseumsetting.h"

namespace GlobalMuseumSetting
{
	bool museum = false;

	bool enabled()
	{
		return museum;
	}

	void setEnabled(bool enable)
	{
		museum = enable;
	}
}

