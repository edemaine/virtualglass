
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <string>
#include <fstream>
#include "primitives.h"

// Mesh stuff
class Vertex
{
        public:
        Vertex(Point const &_position = make_vector(0.0f, 0.0f, 0.0f), Point const &_normal = make_vector(0.0f, 0.0f, 0.0f), Color const &_color = make_vector(1.0f, 1.0f, 1.0f, 1.0f)) : position(_position), normal(_normal), color(_color) {
        }
        Point position;
        Point normal;
        Color color;
};
class Triangle
{
        public:
        Triangle(uint32_t _v1 = -1U, uint32_t _v2 = -1U, uint32_t _v3 = -1U) : v1(_v1), v2(_v2), v3(_v3) {
        }
        uint32_t v1;
        uint32_t v2;
        uint32_t v3;
};

class Geometry
{
        public:
        std::vector< Vertex > vertices;
        std::vector< Triangle > triangles;
        void clear() {
                vertices.clear();
                triangles.clear();
        }
        bool valid() const {
                for (std::vector< Triangle >::const_iterator t = triangles.begin(); t != triangles.end(); ++t) {
                        if (t->v1 >= vertices.size()) return false;
                        if (t->v2 >= vertices.size()) return false;
                        if (t->v3 >= vertices.size()) return false;
                }
                return true;
        }
        void compute_normals_from_triangles();
        void save_obj_file(std::string const &filename) const;
};

#endif

