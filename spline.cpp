
#include <cassert>
#include "spline.h"

Spline :: Spline()
{
	this->reset();
}

void Spline :: reset()
{
	this->vals.clear();
	this->vals.push_back(3.0);
	this->vals.push_back(3.0);
	this->vals.push_back(3.0);
	this->vals.push_back(3.0);
}

void Spline :: set(unsigned int i, float v)
{
	if (i < this->vals.size())
		this->vals[i] = v;
}

const vector<float>& Spline :: values()
{
	return this->vals;
}

void Spline :: addValue()
{
	this->vals.push_back(3.0);
}

void Spline :: removeValue()
{
	if (this->vals.size() > 4)
		this->vals.pop_back();
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
	for (unsigned int i = 1; i <= p_power; ++i)
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
	unsigned int n = this->vals.size()-1;
	for (unsigned int i = 0; i <= n; ++i)
		val += choose(n, i) * pow((1.0 - t), n - i) * pow(t, i) * this->vals[i];
	return val; 
}


