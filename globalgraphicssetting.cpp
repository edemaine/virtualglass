
#include "globalgraphicssetting.h"

namespace GlobalGraphicsSetting
{
	enum Setting setting;

	enum Setting get()
	{
		return setting;
	}

	unsigned int getQuality()
	{
		return static_cast<unsigned int>(setting);
	}	

	void set(enum Setting s)
	{
		setting = s;
	}
}

