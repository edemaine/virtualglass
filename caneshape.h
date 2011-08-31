
#ifndef CANESHAPE_H
#define CANESHAPE_H

#include <vector>
#include "geometry.h"

using std::vector;

class CaneShape
{
	public:
		CaneShape();
		vector< Vector2f > const &getVertices();
		int getType();
		float getDiameter();
		void setVertices(vector< Vector2f > const &newVertices);
		void setType(int type);
		void setDiameter(float newDiameter);
		void setByTypeAndDiameter(int type, float diameter, int resolution);
		void setByVertices(vector< Vector2f > const &vertices);
		void clear();
		void copy(CaneShape* dest);

	//private: //I'm nationalizing this class's data. --Jim
		vector< Vector2f > vertices;
		//if empty, assume to be a triangle fan over vertices.
		vector< Vector3ui > tris; //cap triangulation
		int type; // should be CIRCLE_SHAPE, SQUARE_SHAPE, etc.
		float diameter;	

};

#endif 


