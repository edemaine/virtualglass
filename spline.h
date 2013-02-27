
#ifndef SPLINE_H
#define SPLINE_H

#include <vector>
#include "primitives.h"

using std::vector;

class Spline
{
	public:
		Spline();
		void reset();
		Point2D get(float t);
		const vector<Point2D>& controlPoints();
		void addPoint(Point2D p);
		void removePoint();
		void set(unsigned int i, Point2D c);

	private:
		static float pow(float base, int power);
		static unsigned int choose(unsigned int n, unsigned int k);
		vector<Point2D> controls;
};

#endif

