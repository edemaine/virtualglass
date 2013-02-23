
#include <cassert>
#include "spline.h"

Spline :: Spline()
{
	this->values.push_back(1.0);
	this->values.push_back(1.0);
	this->values.push_back(1.0);
	this->values.push_back(1.0);
}

Spline :: Spline(vector<float> _vals)
{
	this->values = _vals;
}

float Spline :: start()
{
	assert(this->values.size() > 0);
	return this->values.front();
}

float Spline :: end()
{
	assert(this->values.size() > 0);
	return this->values.back();
}

unsigned int Spline :: choose(unsigned int n, unsigned int k)
{
	if (n < k)
		return 0;
	unsigned int total = 1;
	for (unsigned int i = n; i > n - k; --i)
		total *= i; // get the numerator
	for (unsigned int i = k; i > 0; --i)
		total /= i; // get the denominator
	return total;
}

float Spline :: pow(float base, int power)
{
	unsigned int p_power;
	if (power < 0)
		p_power = -1 * power;
	else 
		p_power = power;

	float total = 1.0;
	for (unsigned int i = 0; i < p_power; ++i)
		total *= base;
	
	if (power < 0)
		return 1.0 / total;
	else
		return total;
}

float Spline :: get(float t)
{
	assert(-0.1 < t && t < 1.1); // if you're outside this, you're probably a bug

	float val = 0;
	for (unsigned int i = 0; i < this->values.size(); ++i)
		val += choose(this->values.size()-1, i) 
			* pow((1.0 - t), this->values.size() - 1 - i) 
			* pow(t, i) * this->values[i];
	return val; 
}


