
#ifndef GLASSMIME_H
#define GLASSMIME_H

namespace GlassMime
{
	enum Type
	{
		colorbar,
		pullplan
	};

	void encode(char* mimeData, void* ptr, enum Type type);

	void decode(const char* mimeData, void** ptr, enum Type* type);
}


#endif



