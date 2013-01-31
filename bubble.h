#ifndef BUBBLE_H
#define BUBBLE_H


class Bubble
{
public:
	Bubble(float radius = 1, unsigned int rings = 12, unsigned int sectors = 24);
	void draw(GLfloat, GLfloat, GLfloat);
	void display(Bubble);

	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	std::vector<GLfloat> texcoords;
	std::vector<GLushort> indices;


};

#endif // BUBBLE_H
