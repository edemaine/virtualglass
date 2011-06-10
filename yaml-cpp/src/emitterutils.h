#ifndef EMITTERUTILS_H_62B23520_7C8E_11DE_8A39_0800200C9A66
#define EMITTERUTILS_H_62B23520_7C8E_11DE_8A39_0800200C9A66

#if !defined(__GNUC__) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || (__GNUC__ >= 4) // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif


#include "yaml-cpp/ostream.h"
#include <string>

namespace YAML
{
	namespace Utils
	{
		bool WriteString(ostream& out, const std::string& str, bool inFlow, bool escapeNonAscii);
		bool WriteSingleQuotedString(ostream& out, const std::string& str);
		bool WriteDoubleQuotedString(ostream& out, const std::string& str, bool escapeNonAscii);
		bool WriteLiteralString(ostream& out, const std::string& str, int indent);
		bool WriteComment(ostream& out, const std::string& str, int postCommentIndent);
		bool WriteAlias(ostream& out, const std::string& str);
		bool WriteAnchor(ostream& out, const std::string& str);
		bool WriteTag(ostream& out, const std::string& str, bool verbatim);
		bool WriteTagWithPrefix(ostream& out, const std::string& prefix, const std::string& tag);
		bool WriteBinary(ostream& out, const char *data, std::size_t size);
	}
}

#endif // EMITTERUTILS_H_62B23520_7C8E_11DE_8A39_0800200C9A66
