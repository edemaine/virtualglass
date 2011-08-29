

#include "caneshape.h"

CaneShape :: CaneShape()
{
	clear();
}

void CaneShape :: clear()
{
	vertices.clear();
	type = UNDEFINED_SHAPE;
	diameter = -1;
}

void CaneShape :: copy(CaneShape* dest)
{
	dest->setVertices(this->getVertices());
	dest->setType(this->getType());
	dest->setDiameter(this->getDiameter());
}

void CaneShape :: setVertices(vector<Point> vertices)
{
	this->vertices.clear();
	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		this->vertices.push_back(vertices[i]);
	}
}

void CaneShape :: setType(int type)
{
	this->type = type;
}

void CaneShape :: setDiameter(float diameter)
{
	this->diameter = diameter;
}


vector<Point> CaneShape :: getVertices()
{
	return vertices;
}

int CaneShape :: getType()
{
	return type;
}

float CaneShape :: getDiameter()
{
	return diameter;
} 

void CaneShape :: setByVertices(vector<Point> vertices)
{
	setVertices(vertices);
	type = UNDEFINED_SHAPE;
	diameter = -1.0;
}

void CaneShape :: setByTypeAndDiameter(int type, float diameter, int resolution)
{
	clear();
	setType(type);
	setDiameter(diameter);
	Point p;
	float t;
	vector<Point> vertices;
	vertices.clear();
	switch (type)
	{
		case CIRCLE_SHAPE:
			for (int i = 0; i < resolution; ++i)
			{
				p.x = diameter * 0.5 * cos(2 * PI * i / resolution);
				p.y = diameter * 0.5 * sin(2 * PI * i / resolution);
				vertices.push_back(p);
			}
			break;
		case SQUARE_SHAPE:
			for (int i = 0; i < resolution / 4; ++i)
			{
				p.x = diameter * 0.5;
				p.y = diameter * (-0.5 + 1.0 * 4 * i / resolution);
				vertices.push_back(p);
			}
			for (int i = 0; i < resolution / 4; ++i)
			{
				p.x = diameter * (0.5 - 1.0 * 4 * i / resolution);
				p.y = diameter * 0.5;
				vertices.push_back(p);
			}
			for (int i = 0; i < resolution / 4; ++i)
			{
				p.x = diameter * -0.5;
				p.y = diameter * (0.5 - 1.0 * 4 * i / resolution);
				vertices.push_back(p);
			}
			for (int i = 0; i < resolution / 4; ++i)
			{
				p.x = diameter * (-0.5 + 1.0 * 4 * i / resolution);
				p.y = diameter * -0.5;
				vertices.push_back(p);
			}
			break;
		case RECTANGLE_SHAPE:
			for (int i = 0; i < resolution / 4; ++i)
			{
				p.x = diameter * 0.5;
				p.y = diameter * (-0.25 + 0.5 * 4 * i / resolution);
				vertices.push_back(p);
			}
			for (int i = 0; i < resolution / 4; ++i)
			{
				p.x = diameter * (0.5 - 1.0 * 4 * i / resolution);
				p.y = diameter * 0.25;
				vertices.push_back(p);
			}
			for (int i = 0; i < resolution / 4; ++i)
			{
				p.x = diameter * -0.5;
				p.y = diameter * (0.25 - 0.5 * 4 * i / resolution);
				vertices.push_back(p);
			}
			for (int i = 0; i < resolution / 4; ++i)
			{
				p.x = diameter * (-0.5 + 1.0 * 4 * i / resolution);
				p.y = diameter * -0.25;
				vertices.push_back(p);
			}
			break;
		case TRIANGLE_SHAPE:
			for (int i = 0; i < resolution / 3; ++i)
			{
				t = 3.0 * i / resolution;
				p.x = diameter * (1 * (1 - t) + cos(2 * PI / 3) * t);
				p.y = diameter * (0 * (1 - t) + sin(2 * PI / 3) * t);
				vertices.push_back(p);
			}
			for (int i = 0; i < resolution / 3; ++i)
			{
				t = 3.0 * i / resolution;
				p.x = diameter * (cos(2 * PI / 3) * (1 - t) + cos(4 * PI / 3) * t);
				p.y = diameter * (sin(2 * PI / 3) * (1 - t) + sin(4 * PI / 3) * t);
				vertices.push_back(p);
			}
			for (int i = 0; i < resolution / 3; ++i)
			{
				t = 3.0 * i / resolution;
				p.x = diameter * (cos(4 * PI / 3) * (1 - t) + 1 * t);
				p.y = diameter * (sin(4 * PI / 3) * (1 - t) + 0 * t);
				vertices.push_back(p);
			}
			break;
		case THIRD_CIRCLE_SHAPE:
			for (int i = 0; i < resolution / 4; ++i)
			{
				p.x = 0;
				p.y = diameter * (1.0 - 1.0 * 4 * i / resolution);
				vertices.push_back(p);
			}
			for (int i = 0; i < resolution / 4; ++i)
			{
				p.x = diameter * (cos(-PI / 6) * i * 4.0 / resolution);
				p.y = diameter * (sin(-PI / 6) * i * 4.0 / resolution);
				vertices.push_back(p);
			}
			for (int i = 0; i < resolution / 2; ++i)
			{
				p.x = diameter * cos(-PI / 6 + 2 * PI / 3 * i * 2 / resolution);
				p.y = diameter * sin(-PI / 6 + 2 * PI / 3 * i * 2 / resolution);
				vertices.push_back(p);
			}
			break;
		case HALF_CIRCLE_SHAPE:
			for (int i = 0; i < resolution / 3; ++i)
			{
				p.x = 0;
				p.y = diameter * (1.0 - 2.0 * 3 * i / resolution);
				vertices.push_back(p);
			}
			for (int i = 0; i < 2 * resolution / 3; ++i)
			{
				p.x = diameter * cos(-PI/2 + PI * 3 * i / (2 * resolution));
				p.y = diameter * sin(-PI/2 + PI * 3 * i / (2 * resolution));
				vertices.push_back(p);
			}
			break;
	}
	setVertices(vertices);
}

