
#include <cassert>
#include "spline.h"

Spline :: Spline()
{
	this->reset();
}

void Spline :: reset()
{
	this->controls.clear();
	Point2D p;
	p.x = p.y = 0.0;
	this->controls.push_back(p);
	this->controls.push_back(p);
}

void Spline :: set(unsigned int i, Point2D c)
{
	if (i < this->controls.size())
		this->controls[i] = c;
}

const vector<Point2D>& Spline :: controlPoints()
{
	return this->controls;
}

void Spline :: addPoint(Point2D p)
{
	this->controls.push_back(p);
}

void Spline :: removePoint()
{
	if (this->controls.size() > 2)
		this->controls.pop_back();
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

Point2D Spline :: get(float t)
{
	assert(-0.1 < t && t < 1.1); // if you're outside this, you're probably a bug

	Point2D p;
	p.x = p.y = 0.0;
	unsigned int n = this->controls.size()-1;
	for (unsigned int i = 0; i <= n; ++i)
	{
		float coefficient = choose(n, i) * pow((1.0 - t), n - i) * pow(t, i);
		p.x += coefficient * this->controls[i].x;
		p.y += coefficient * this->controls[i].y;
	}
	return p; 
}


