
#include <stdio.h>
#include "glassmime.h"

namespace GlassMime
{
	void encode(char* mimeData, void* ptr, enum Type type)
	{
		sprintf(mimeData, "%p %d", ptr, type);
	}

	void decode(const char* mimeData, void** ptr, enum Type* type)
	{
		int tmp; 
		sscanf(mimeData, "%p %d", ptr, &tmp);
		*type = static_cast<enum Type>(tmp);
	}
}

