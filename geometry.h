
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <string>
#include "primitives.h"
#include "pullplan.h"

// Commented out to turn off validity checks of geometry
// #define GEOMETRY_DEBUG

class Cane;

// Mesh stuff
class Vertex
{
public:
	Vertex(Point const &_position = make_vector(0.0f, 0.0f, 0.0f), 
		Point const &_normal = make_vector(0.0f, 0.0f, 0.0f)) : position(_position), normal(_normal) {}
	Point position;
	Point normal;
};

class Triangle
{
public:
	Triangle(uint32_t _v1 = -1U, uint32_t _v2 = -1U, uint32_t _v3 = -1U) : v1(_v1), v2(_v2), v3(_v3) {}
	uint32_t v1;
	uint32_t v2;
	uint32_t v3;
};


//groups are a (hack-y) way of tracking triangles that belong to specific canes
class Group
{
public:
	Group(uint32_t _triangle_begin, uint32_t _triangle_size, uint32_t _vertex_begin, uint32_t _vertex_size, 
		Color *_color, bool _ensureVisible, uint32_t _tag)
		: triangle_begin(_triangle_begin), triangle_size(_triangle_size), vertex_begin(_vertex_begin), 
		vertex_size(_vertex_size), color(_color), ensureVisible(_ensureVisible), tag(_tag) {
	}
	uint32_t triangle_begin;
	uint32_t triangle_size;
	uint32_t vertex_begin;
	uint32_t vertex_size;
	Color* color;
	bool ensureVisible;
	uint32_t tag;
};

class Geometry
{
public:
	std::vector< Vertex > vertices;
	std::vector< Triangle > triangles;
	std::vector< Group > groups;
	void clear() {
		vertices.clear();
		triangles.clear();
		groups.clear();
	}
	bool valid() const
	{
#ifndef GEOMETRY_DEBUG
		return true;
#else
		for (std::vector< Triangle >::const_iterator t = triangles.begin(); t != triangles.end(); ++t) {
			if (t->v1 >= vertices.size()) return false;
			if (t->v2 >= vertices.size()) return false;
			if (t->v3 >= vertices.size()) return false;
		}
		for (std::vector< Group >::const_iterator g = groups.begin(); g != groups.end(); ++g) {
			if (g->triangle_begin >= triangles.size()) return false;
			if (g->triangle_begin + g->triangle_size > triangles.size()) return false;
			if (g->color == NULL) return false;
			if (g->vertex_begin >= vertices.size()) return false;
			if (g->vertex_begin + g->vertex_size > vertices.size()) return false;
		}
		return true;
#endif
	}
	void compute_normals_from_triangles();
	void save_obj_file(std::string const &filename) const;
	void save_raw_file(std::string const &filename) const;
};

#endif

