
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <string>
#include "primitives.h"
#include "pullplan.h"

class Cane;

// Mesh stuff
class Vertex
{
	public:
		Vertex(Point const &_position, Point const &_normal);
		Point position;
		Point normal;
};

class Triangle
{
	public:
		Triangle(uint32_t _v1, uint32_t _v2, uint32_t _v3);
		uint32_t v1;
		uint32_t v2;
		uint32_t v3;
};


//groups are a (hack-y) way of tracking triangles that belong to specific canes
class Group
{
	public:
		Group(uint32_t _triangle_begin, uint32_t _triangle_size, uint32_t _vertex_begin, uint32_t _vertex_size, 
			Color _color, bool _ensureVisible=false);
		uint32_t triangle_begin;
		uint32_t triangle_size;
		uint32_t vertex_begin;
		uint32_t vertex_size;
		Color color;
		bool ensureVisible;
};

class Geometry
{
	public:
		std::vector< Vertex > vertices;
		std::vector< Triangle > triangles;
		std::vector< Group > groups;
		void clear();
		bool valid() const;
		void compute_normals_from_triangles();
		void save_obj_file(std::string const &filename) const;
		void save_ply_file(std::string const &filename) const;
		void save_raw_file(std::string const &filename) const;
};

#endif

