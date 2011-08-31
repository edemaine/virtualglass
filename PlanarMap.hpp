#ifndef PLANAR_MAP_HPP
#define PLANAR_MAP_HPP

#include "Vector.hpp"
#include "Box.hpp"

#include <vector>

class MTRand;

class PlanarMap {
public:

	//Driver routines:
	static void fill(int fill_rule, std::vector< std::vector< Vector2d > > const &paths, Box2d const &bounds, std::vector< Vector2d > &points, std::vector< Vector3ui > &triangles);
	static void stroke(double width, std::vector< std::vector< Vector2d > > const &paths, Box2d const &bounds, std::vector< Vector2d > &points, std::vector< Vector3ui > &triangles);

	enum {
		EvenOdd = 0,
		NonZero = 1,
	};
	int fill_rule;

	PlanarMap(std::vector< std::vector< Vector2i > > const &oriented_paths, int fill_rule = EvenOdd, unsigned int seed = 0x42424241);
	~PlanarMap();

	std::vector< std::vector< unsigned int > > outs;

	std::vector< Vector2i > points;
	std::vector< Vector2ui > edges;
	std::vector< int > edge_counts;

	std::vector< std::vector< unsigned int > > loops;
	std::vector< unsigned int > loop_faces;
	std::vector< std::vector< unsigned int > > face_loops;
	std::vector< int > face_counts;

	std::vector< std::vector< Vector3ui > > face_triangles;

	MTRand *mt_rand;

	size_t add_point(Vector2i p);
	void add_edge(Vector2i a, Vector2i b, int count);
	void compute_outs();
	bool peel_loop();
	void do_face_counts();
	bool triangulate_face();
	//called by triangulate_face():
	void peel_loop(std::vector< unsigned int > const &loop, std::vector< Vector3ui > &tris);

};

#endif //PLANAR_MAP_HPP
