/*
 * (unfinished) SVG parsing code from an (unfinished) game Jim has
 * been working on. This code is released into the public domain.
 */
#ifndef SVG_HPP
#define SVG_HPP

#include <Vector/Vector.hpp>
#include <Vector/Box.hpp>
#include <Vector/Matrix.hpp>

#include <list>
#include <string>
#include <vector>

namespace SVG {

typedef Matrix< double, 2, 3 > Matrix;

class Paint {
public:
	Paint() : color(make_vector(1.0f, 0.0f, 1.0f, 1.0f)) {
	}
	Vector4f color;
	//TODO: support gradients
};

class Node {
public:
	Node() : tag(""), transform(identity_matrix< double, 2, 3 >()), stroke_width(1.0), fill_rule(FILL_EVENODD) {
	}
	//tag read from file:
	std::string tag;
	//Transform to parent coords (applies to both path and children):
	Matrix transform;
	//Style information:
	Paint stroke_paint;
	double stroke_width;
	Paint fill_paint;
	int fill_rule;
	enum {
		FILL_EVENODD = 0,
		FILL_NONZERO = 1,
	};
	void copy_style(Node &other) {
		stroke_paint = other.stroke_paint;
		stroke_width = other.stroke_width;
		fill_paint = other.fill_paint;
		fill_rule = other.fill_rule;
	}
	//Path info:
	//(represented in an svg-ish way, but without
	// relative coords, crazy arc syntax, or shorthand/quadratic Beziers)
	std::vector< char > ops;
	enum {
		OP_MOVETO = 'M', //consumes 1 coord
		OP_LINETO = 'L', //consumes 1 coord
		OP_CLOSEPATH = 'Z', //consumes 0 coords
		OP_CURVETO = 'C', //cubic bezier curve, consumes 3 coords
		OP_ARCTO = 'A', //arc, consumes 2 coords (midpoint,end)
		//Note that several ARCTOs might result from one SVG arc command,
		// as this ARCTO is using my midpoint notation.
		//Note also that I'm not entirely sure that midpoint notation
		// can actually produce elliptical arcs properly.
	};
	std::vector< Vector2d > coords;
	void moveto(Vector2d const &pos) {
		ops.push_back('M');
		coords.push_back(pos);
	}
	void lineto(Vector2d const &pos) {
		ops.push_back('L');
		coords.push_back(pos);
	}
	void closepath() {
		ops.push_back('Z');
	}
	//'tol' is subdivision tolerance, after xform. (Output curve within 'tol' of true curve.)
	//paths will start/end with same point if closed
	void execute(Matrix const &xform, double tol, std::vector< std::vector< Vector2d > > &paths) const;
	//Child nodes:
	std::list< Node > children;
};

class SVG {
public:
	SVG() : page(make_vector(0.0, 0.0)) {
	}
	Vector2d page;
	Node root;
};

bool load_svg(std::string const &filename, SVG &into);

};

#endif //SVG_HPP
