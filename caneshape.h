
#ifndef CANESHAPE_H
#define CANESHAPE_H

#include <vector>
#include "geometry.h"

using std::vector;

class CaneShape
{
	public:
		CaneShape();
		vector<Point> getVertices();
		int getType();
		float getDiameter();
		void setVertices(vector<Point> newVertices);
		void setType(int type);
		void setDiameter(float newDiameter);
		void setByTypeAndDiameter(int type, float diameter, int resolution);
		void setByVertices(vector<Point> vertices);
		void clear();
		void copy(CaneShape* dest);

	private:
		vector<Point> vertices;
		int type; // should be CIRCLE_SHAPE, SQUARE_SHAPE, etc.
		float diameter;	

};

#endif 


