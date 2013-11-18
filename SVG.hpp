#ifndef SVG_HPP
#define SVG_HPP

#include "Vector.hpp"
#include "Box.hpp"
#include "Matrix.hpp"
#include "cane.h"
#include "globalglass.h"

#include <list>
#include <string>
#include <vector>

namespace SVG {

typedef Matrix< double, 2, 3 > Matrix;

class Paint {
public:
	Paint() : color(make_vector(1.0f, 0.0f, 1.0f, 1.0f)), opacity(1.0f) {
	}
	Vector4f color;
	float opacity;
	//TODO: support gradients
	Vector4ub to_gl_color() const {
		Vector4ub ret;
		for (unsigned int i = 0; i < 4; ++i) {
			int val;
			if (i != 3) {
				val = color.c[i] * 256;
			} else {
				val = color.c[i] * opacity * 256;
			}
			if (val < 0) val = 0;
			if (val > 255) val = 255;
			ret.c[i] = val;
		}
		return ret;
	}
};

class ArcInfo {
public:
	ArcInfo(bool _large_arc_flag, bool _sweep_flag) : large_arc_flag(_large_arc_flag), sweep_flag(_sweep_flag) {
	}
	bool large_arc_flag;
	bool sweep_flag;
};

class Node {
public:
	Node() : tag(""), transform(identity_matrix< double, 2, 3 >()), stroke_width(1.0), fill_rule(FILL_NONZERO) {
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
		OP_ARCTO = 'A', //arc, consumes 3 coords (x axis, y axis, endpoint) and 1 arc_info
	};
	std::vector< Vector2d > coords;
	std::vector< ArcInfo > arc_infos;
	void moveto(Vector2d const &pos) {
		ops.push_back(OP_MOVETO);
		coords.push_back(pos);
	}
	void lineto(Vector2d const &pos) {
		ops.push_back(OP_LINETO);
		coords.push_back(pos);
	}
	void arcto(Vector2d const &x_axis, Vector2d const &y_axis, ArcInfo const &info, Vector2d const &to) {
		ops.push_back(OP_ARCTO);
		arc_infos.push_back(info);
		coords.push_back(x_axis);
		coords.push_back(y_axis);
		coords.push_back(to);
	}
	void curveto(Vector2d const &c1, Vector2d const &c2, Vector2d const &end) {
		ops.push_back(OP_CURVETO);
		coords.push_back(c1);
		coords.push_back(c2);
		coords.push_back(end);
	}
	void closepath() {
		ops.push_back(OP_CLOSEPATH);
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

bool load_svg(std::string const &filename, SVG &into, Cane *cane);

};

#endif //SVG_HPP
