
#include "globalgraphicssetting.h"

namespace GlobalGraphicsSetting
{
	enum Setting setting;

	enum Setting get()
	{
		return setting;
	}

	void set(enum Setting s)
	{
		setting = s;
	}
}

