#ifndef CONVERSION_H_62B23520_7C8E_11DE_8A39_0800200C9A66
#define CONVERSION_H_62B23520_7C8E_11DE_8A39_0800200C9A66

#if !defined(__GNUC__) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || (__GNUC__ >= 4) // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif


#include "yaml-cpp/null.h"
#include "yaml-cpp/traits.h"
#include <string>
#include <sstream>

namespace YAML
{
	inline bool Convert(const std::string& input, std::string& output) {
		output = input;
		return true;
	}
	
	YAML_CPP_API bool Convert(const std::string& input, bool& output);
	YAML_CPP_API bool Convert(const std::string& input, _Null& output);
	
	template <typename T> 
	inline bool Convert(const std::string& input, T& output, typename enable_if<is_numeric<T> >::type * = 0) {
		std::stringstream stream(input);
		stream.unsetf(std::ios::dec);
		stream >> output;
		return !!stream;
	}
}

#endif // CONVERSION_H_62B23520_7C8E_11DE_8A39_0800200C9A66
