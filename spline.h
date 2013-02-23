
#ifndef SPLINE_H
#define SPLINE_H

#include <vector>

using std::vector;

class Spline
{
	public:
		Spline();
		Spline(vector<float> vals);
		float get(float t);
		vector<float> values;
		float start();		
		float end();

	private:
		static float pow(float base, int power);
		static unsigned int choose(unsigned int n, unsigned int k);
};

#endif

