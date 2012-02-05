/*
Tests for a nasty LLVM-gcc bug that was causing problems in VirtualGlass on OSX:
Running:
g++ -g -O0 -Wall -Werror -o test_llvm_bug test_llvm_bug.cpp && ./test_llvm_bug
Produces:
Assertion failed: (arg == 0x11223344), function glob_pushNewSubpullLikeFunction, file test_main.cpp, line 16.
Abort trap
*/

#include <cassert>

class Point {
public:
	union {
		float a[3];
		struct {
			float pad;
			float b[2];
		};
	};
};

inline void glob_pushNewSubpullLikeFunction(int arg, Point /* ignored */) {
	assert(arg == 0x11223344);
}

int main(int argc, char** argv)
{
	Point fred;
	fred.a[0] = 1.0f;
	fred.a[1] = 2.0f;
	fred.a[2] = 3.0f;
	glob_pushNewSubpullLikeFunction(0x11223344, fred);
	return 0;
}


