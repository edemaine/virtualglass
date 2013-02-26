
#ifndef SPLINE_H
#define SPLINE_H

#include <vector>

using std::vector;

class Spline
{
	public:
		Spline();
		void reset();
		float get(float t);
		const vector<float>& values();
		void addValue();
		void removeValue();
		void set(unsigned int i, float v);

	private:
		static float pow(float base, int power);
		static unsigned int choose(unsigned int n, unsigned int k);
		vector<float> vals;
};

#endif

