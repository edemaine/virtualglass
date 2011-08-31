#include "PlanarMap.hpp"
#include "MersenneTwister.hpp"

#include "Matrix.hpp"

#include <algorithm>

#ifdef UNORDERED_MAP_WORKAROUND
#include <tr1/unordered_map>
using std::tr1::unordered_map;
#else
#include <unordered_map>
using std::unordered_map;
#endif


using std::vector;
using std::make_pair;

#define LOG_ERROR( X ) std::cerr << X << std::endl
#define LOG_WARNING( X ) std::cout << X << std::endl
#define LOG_INFO( X ) std::cout << X << std::endl


namespace {

Vector2i round(Vector2d const &v) {
	Vector2i ret;
	if (v.x >= 0.0) {
		ret.x = (int)(v.x + 0.5);
	} else {
		ret.x = (int)(v.x - 0.5);
	}
	if (v.y >= 0.0) {
		ret.y = (int)(v.y + 0.5);
	} else {
		ret.y = (int)(v.y - 0.5);
	}
	return ret;
}

int round_div(int num, int den) {
	assert(den != 0);
	if (den < 0) {
		num = -num;
		den = -den;
	}
	if (num >= 0) {
		return (num + den / 2) / den;
	} else {
		return (num - den / 2) / den;
	}
}

bool point_on_line(Vector2i p, Vector2i a, Vector2i b) {
	//does 'p' lie on a-b?
	//t * (b - a) = (p - a) for t between zero and one?
	//t = (p.x - a.x) / (b.x - a.x)
	if (b.x == a.x) {
		int t_num = p.y - a.y;
		int t_den = b.y - a.y;
		if (t_den < 0) {
			t_num = -t_num;
			t_den = -t_den;
		}
		if (t_num >= 0 && t_num <= t_den) {
			if ((p.x - a.x) * t_den == (b.x - a.x) * t_num) {
				return true;
			}
		}
	} else {
		int t_num = p.x - a.x;
		int t_den = b.x - a.x;
		if (t_den < 0) {
			t_num = -t_num;
			t_den = -t_den;
		}
		//check if p.y - a.y == t * (b.y - a.y)
		if (t_num >= 0 && t_num <= t_den) {
			if ((p.y - a.y) * t_den == (b.y - a.y) * t_num) {
				return true;
			}
		}
	}
	return false;
}

//is point p left (1), right (-1), or on (0) a-b?
int line_side(Vector2i p, Vector2i a, Vector2i b) {
	int amt = (p - a) * perpendicular(b - a);
	if (amt < 0) {
		return -1;
	} else if (amt > 0) {
		return 1;
	} else {
		return 0;
	}
}

} //end anon namespace

void PlanarMap::fill(int fill_rule, vector< vector< Vector2d > > const &paths, Box2d const &bounds, vector< Vector2d > &points, vector< Vector3ui > &triangles) {

	double scale = 1.0;
	const int MaxDiam = (1 << 14);
	if (bounds.size().x > bounds.size().y) {
		if (bounds.size().x > 0.0) {
			scale = MaxDiam / bounds.size().x;
		}
	} else {
		if (bounds.size().y > 0.0) {
			scale = MaxDiam / bounds.size().y;
		}
	}

	vector< vector< Vector2i > > pts(paths.size());
	for (unsigned int p = 0; p < paths.size(); ++p) {
		pts[p].resize(paths[p].size());
		for (unsigned int i = 0; i < paths[p].size(); ++i) {
			pts[p][i] = round((paths[p][i] - bounds.center()) * scale);
			if (pts[p][i].x > MaxDiam || pts[p][i].x < -MaxDiam || pts[p][i].y > MaxDiam || pts[p][i].y < -MaxDiam) {
				LOG_ERROR("Point outside MaxDiam -- could be a numerical issue later.");
			}
		}
		//close paths for fill operation:
		if (!pts[p].empty() && pts[p][0] != pts[p].back()) {
			pts[p].push_back(pts[p][0]);
		}
	}

	PlanarMap pm(pts, fill_rule);

	size_t ofs = points.size();
	points.resize(points.size() + pm.points.size());
	for (unsigned int p = 0; p < pm.points.size(); ++p) {
		points[ofs + p] = make_vector< double >(pm.points[p]) / scale + bounds.center();
	}
	for (vector< vector< Vector3ui > >::iterator ft = pm.face_triangles.begin(); ft != pm.face_triangles.end(); ++ft) {
		for (vector< Vector3ui >::iterator t = ft->begin(); t != ft->end(); ++t) {
			triangles.push_back(*t + make_vector< unsigned int >(ofs, ofs, ofs));
		}
	}

}

namespace {
//intersect [a1,b1] and [a2,b2], store in 'p'
// an inexact solution, to be sure.
bool isect(Vector2d const &a1, Vector2d const &b1, Vector2d const &a2, Vector2d const &b2, Vector2d &p) {
	double amt_a = perpendicular(b2 - a2) * (a1 - a2);
	double amt_b = perpendicular(b2 - a2) * (b1 - a2);
	if (amt_a > 0.0 && amt_b > 0.0) return false;
	if (amt_a < 0.0 && amt_b < 0.0) return false;
	//want t such that t * (amt_b - amt_a) + amt_a = 0
	double t = -amt_a / (amt_b - amt_a);
	double along_a = (b2 - a2) * (a1 - a2);
	double along_b = (b2 - a2) * (b1 - a2);
	double along = along_a + t * (along_b - along_a);
	//flipped conditions to weed out any NaN-ish behaviour which may creep in:
	if (!(along >= 0.0)) return false;
	if (!(along <= length_squared(b2 - a2))) return false;
	along /= length_squared(b2 - a2);
	p = along * (b2 - a2) + a2;
	return true;
}
};

void PlanarMap::stroke(double width, std::vector< std::vector< Vector2d > > const &paths, Box2d const &bounds, std::vector< Vector2d > &points, std::vector< Vector3ui > &triangles) {
	if (width == 0.0) return;
	vector< vector< Vector2d > > stroke_paths;
	for (vector< vector< Vector2d > >::const_iterator src = paths.begin(); src != paths.end(); ++src) {
		if (src->size() < 2) continue;
		vector< Vector2d > unique;
		unique.push_back((*src)[0]);
		for (vector< Vector2d >::const_iterator v = src->begin() + 1; v != src->end(); ++v) {
			if (*v != unique.back()) {
				unique.push_back(*v);
			}
		}
		if (unique.size() < 2) continue;
		bool is_loop = false; //DEBUG! was: (unique[0] == unique.back());
		Vector2d prev;
		if (is_loop) {
			//we just set previous properly:
			prev = unique[unique.size()-2];
			assert(0); //DEBUG
		} else {
			//we hallucinate some past-the-end stuff:
			prev = 2.0 * unique[0] - unique[1];
			unique.push_back(2.0 * unique.back() - unique[unique.size()-2]);
		}
		Vector2d cur = unique[0];
		Vector2d pc_perp = normalize(perpendicular(cur - prev));

		vector< Vector2d > outer;
		vector< Vector2d > inner;
		for (unsigned int i = 1; i < unique.size(); ++i) {
			Vector2d next = unique[i];
			Vector2d cn_perp = normalize(perpendicular(next - cur));

			{ //check for outer bend vert:
				Vector2d bend;
				if (isect(prev - 0.5 * width * pc_perp, cur - 0.5 * width * pc_perp, cur - 0.5 * width * cn_perp, next - 0.5 * width * cn_perp, bend)) {
					outer.push_back(bend);
				} else {
					outer.push_back(cur - 0.5 * width * pc_perp);
					outer.push_back(cur - 0.5 * width * cn_perp);
				}
			}
			{ //check for inner bend vert:
				Vector2d bend;
				if (isect(prev + 0.5 * width * pc_perp, cur + 0.5 * width * pc_perp, cur + 0.5 * width * cn_perp, next + 0.5 * width * cn_perp, bend)) {
					inner.push_back(bend);
				} else {
					inner.push_back(cur + 0.5 * width * pc_perp);
					inner.push_back(cur + 0.5 * width * cn_perp);
				}
			}

			prev = cur;
			cur = next;
			pc_perp = cn_perp;
		}
		if (is_loop) {
			//For loop, we treat inner and outer loops separately:
			std::reverse(inner.begin(), inner.end());
			inner.push_back(inner[0]);
			outer.push_back(outer[0]);
			stroke_paths.push_back(inner);
			stroke_paths.push_back(outer);
			assert(0); //DEBUG
		} else {
			//For strip, we weld the inner and outer strokes together:
			inner.insert(inner.end(), outer.rbegin(), outer.rend());
			stroke_paths.push_back(inner);
		}

	}
	Box2d stroke_bounds = bounds;
	stroke_bounds.min -= make_vector(width, width);
	stroke_bounds.max += make_vector(width, width);
	fill(NonZero, stroke_paths, stroke_bounds, points, triangles);
}


PlanarMap::PlanarMap(std::vector< std::vector< Vector2i > > const &oriented_paths, int _fill_rule, unsigned int seed) : fill_rule(_fill_rule), mt_rand(new MTRand(seed)) {
	for (vector< vector< Vector2i > >::const_iterator path = oriented_paths.begin(); path != oriented_paths.end(); ++path) {
		for (unsigned int i = 0; i + 1 < path->size(); ++i) {
			add_edge((*path)[i], (*path)[i+1], 1);
		}
	}
	compute_outs();
	while (peel_loop()) { /* peel 'em all */ }
	do_face_counts();
	while (triangulate_face()) { /* triangulate 'em all */ }
}

PlanarMap::~PlanarMap() {
	delete mt_rand;
	mt_rand = NULL;
}


size_t PlanarMap::add_point(Vector2i p) {
	for (size_t i = 0; i < points.size(); ++i) {
		if (points[i] == p) return i;
	}
	for (vector< Vector2ui >::iterator e = edges.begin(); e != edges.end(); ++e) {
		Vector2i a = points[e->c[0]];
		Vector2i b = points[e->c[1]];
		if (point_on_line(p, a, b)) {
			//split edge at 'p':
			Vector2ui edge = make_vector< unsigned int >(e->c[1], points.size());
			e->c[1] = points.size();
			
			edge_counts.push_back(-edge_counts[e - edges.begin()]);
			edges.push_back(edge);
			points.push_back(p);
			return points.size() - 1;
		}
	}
	points.push_back(p);
	return points.size() - 1;
}

void PlanarMap::add_edge(Vector2i a, Vector2i b, int count) {
	for (vector< Vector2i >::iterator p = points.begin(); p != points.end(); ++p) {
		if (*p == a || *p == b) continue;
		if (point_on_line(*p, a, b)) {
			Vector2i pt = *p;
			add_edge(a,pt, count);
			add_edge(pt,b, count);
			return;
		}
	}
	Vector2ui edge = make_vector< unsigned int >(add_point(a), add_point(b));
	if (edge.c[0] == edge.c[1]) {
		return;
	}
	if (edge.c[0] > edge.c[1]) {
		std::swap(edge.c[0], edge.c[1]);
		std::swap(a, b);
		count *= -1;
	}
	assert(edge_counts.size() == edges.size());
	for (vector< Vector2ui >::iterator e = edges.begin(); e != edges.end(); ++e) {
		Vector2i ea = points[e->c[0]];
		Vector2i eb = points[e->c[1]];
		if (edge == *e) {
			//oh, hey, already exists.
			edge_counts[e - edges.begin()] += count;
			return;
		}
		if (edge.c[0] == e->c[0] || edge.c[0] == e->c[1] || edge.c[1] == e->c[0] || edge.c[1] == e->c[1]) {
			//overlapping endpoints; since we took care of all point-on-line, can ignore.
			continue;
		}
		//we should have taken care of the point-on-line cases already:
		if (line_side(a,ea,eb) == 0 || line_side(b,ea,eb) == 0
		    || line_side(ea,a,b) == 0 || line_side(eb,a,b) == 0) {
			continue;
		}
		if (line_side(a,ea,eb) == line_side(b,ea,eb)) {
			continue;
		}
		if (line_side(ea,a,b) == line_side(eb,a,b)) {
			continue;
		}
		//we've got an intersection!
		//point 'p' such that:
		// (p - a) * perpendicular(b - a)   = 0
		//(p - ea) * perpendicular(eb - ea) = 0
		//p.x * (b.x - a.x) + p.y * (b.y - a.y) = a.x * (b.x - a.x) + a.y * (b.y - a.y)
		//p.x * (eb.x - ea.x) + p.y * (eb.y - ea.y) = ea.x * (eb.x - ea.x) + ea.y * (eb.y - ea.y)
		Matrix< double, 2, 2 > m;
		m(0,0) = perpendicular(b - a).x;
		m(0,1) = perpendicular(b - a).y;
		m(1,0) = perpendicular(eb - ea).x;
		m(1,1) = perpendicular(eb - ea).y;
		double det = m(0,0) * m(1,1) - m(0,1) * m(1,0);
		//inverse is
		//[ m11 -m01] / det
		//[-m10  m00] 
		Vector2d rhs;
		rhs.x = make_vector< double >(a) * make_vector< double >(perpendicular(b - a));
		rhs.y = make_vector< double >(ea) * make_vector< double >(perpendicular(eb - ea));
		Vector2d isect;
		isect.x = m(1,1) * rhs.x + -m(0,1) * rhs.y;
		isect.y =-m(1,0) * rhs.x +  m(0,0) * rhs.y;
		assert(det != 0.0);
		Vector2i iisect = round(isect / det);

		int ecount = edge_counts[e - edges.begin()];
		edge_counts.erase(edge_counts.begin() + (e - edges.begin()));
		edges.erase(e);
		add_edge(a,iisect,count);
		add_edge(iisect,b,count);
		add_edge(ea,iisect,ecount);
		add_edge(iisect,eb,ecount);
		return;
	}
	//Well, if nothing failed for this edge:
	edges.push_back(edge);
	edge_counts.push_back(count);
}

namespace {
//sort things counterclockwise, starting at (1,0).
class CClockwise {
public:
	CClockwise(vector< Vector2i > const &_points, unsigned int _ci) : points(_points), ci(_ci) {
		assert(ci < points.size());
	}
	static void quad_and_slope(Vector2i val, unsigned int &quad, unsigned int &num, unsigned int &den) {
		if (val.y >= 0 && val.x > 0) {
			quad = 0;
			num = val.y;
			den = val.x;
		} else if (val.x <= 0 && val.y > 0) {
			quad = 1;
			num =-val.x;
			den = val.y;
		} else if (val.y <= 0 && val.x < 0) {
			quad = 2;
			num =-val.y;
			den =-val.x;
		} else if (val.x >= 0 && val.y < 0) {
			quad = 3;
			num = val.x;
			den =-val.y;
		} else {
			assert(0);
		}
	}
	bool operator()(unsigned int ai, unsigned int bi) {
		Vector2i c = points[ci];
		Vector2i a = points[ai] - c;
		Vector2i b = points[bi] - c;
		assert(a != make_vector(0,0));
		assert(b != make_vector(0,0));
		unsigned int qa,qb;
		unsigned int na, da, nb, db;
		quad_and_slope(a, qa, na, da);
		quad_and_slope(b, qb, nb, db);
		if (qa < qb) return true;
		if (qa > qb) return false;
		if (na*db < nb*da) return true;
		if (na*db > nb*da) return false;
		return false;
	}

	vector< Vector2i > const &points;
	unsigned int ci;
};
} //end anon namespace

void PlanarMap::compute_outs() {
	outs.clear();
	outs.resize(points.size());
	//unsorted outs:
	for (vector< Vector2ui >::const_iterator e = edges.begin(); e != edges.end(); ++e) {
		assert(e->c[0] < outs.size());
		assert(e->c[1] < outs.size());
		outs[e->c[0]].push_back(e->c[1]);
		outs[e->c[1]].push_back(e->c[0]);
	}
	//sort outs counterclockwise
	for (vector< vector< unsigned int > >::iterator o = outs.begin(); o != outs.end(); ++o) {
		std::sort(o->begin(), o->end(), CClockwise(points, o - outs.begin()));
	}

}

bool PlanarMap::peel_loop() {
	if (outs.empty()) return false;
	//want the uppermost rightmost point which still has outs...
	unsigned int seed = -1U;
	for (unsigned int p = 0; p < points.size(); ++p) {
		if (outs[p].empty()) continue;
		if (seed == -1U) {
			seed = p;
			continue;
		}
		if (points[seed].x < points[p].x) {
			seed = p;
		} else if (points[seed].x == points[p].x && points[seed].y < points[p].y) {
			seed = p;
		}
	}
	if (seed >= points.size()) {
		outs.clear();
		return false;
	}
	assert(!outs[seed].empty());

	vector< unsigned int > loop;
	//LOG_INFO("seed: " << seed); //DEBUG
	loop.push_back(seed);
	loop.push_back(outs[seed][0]);
	while (1) {
		unsigned int at = loop.back();
		unsigned int prev = loop[loop.size()-2];
		//LOG_INFO("prev: " << prev << " -> " << points[prev] << " / at: " << at << " -> " << points[at]); //DEBUG
		assert(!outs[at].empty());
		CClockwise cmp(points, at);
		vector< unsigned int >::iterator o = outs[at].begin();
		while (o != outs[at].end() && !cmp(prev, *o)) {
			++o;
		}
		if (o == outs[at].end()) {
			o = outs[at].begin();
		}
		if (*o == prev) {
			/*
			for (unsigned int i = 0; i < outs[at].size(); ++i) {
				LOG_INFO("outs[at][" << i << "] == " << outs[at][i] << " -> " << points[outs[at][i]]);
			}*/
			assert(outs[at].size() == 1);
		}
		unsigned int next = *o;
		if (at == loop[0] && next == loop[1]) break;
		loop.push_back(next);
	}
	//last point is duplicate of first:
	assert(loop[0] == loop.back());
	loop.pop_back();
	//LOG_INFO("Loop of size " << loop.size());
	//remove the loop from outs:
	for (unsigned int i = 0; i < loop.size(); ++i) {
		unsigned int from = loop[i];
		unsigned int to = loop[(i + 1) % loop.size()];
		bool found = false;
		for (vector< unsigned int >::iterator o = outs[from].begin(); o != outs[from].end(); ++o) {
			if (*o == to) {
				outs[from].erase(o);
				found = true;
				break;
			}
		}
		assert(found);
	}

	assert(!loop.empty());
	loops.push_back(loop);


	//create outer face if not already created.
	if (face_loops.empty()) {
		face_loops.push_back(vector< unsigned int >());
	}

	//Figure out if loop is outside or inside
	vector< unsigned int > trimmed = loop;
	{ //NOTE: this can be done in a more clever (that is, linear) way:
		bool did_cut = false;
		do {
			did_cut = false;
			for (unsigned int i = 0; i < trimmed.size(); ++i) {
				if (trimmed[i] == trimmed[(i+2) % trimmed.size()]) {
					trimmed.erase(trimmed.begin() + i);
					trimmed.erase(trimmed.begin() + (i % trimmed.size()));
					did_cut = true;
					break;
				}
			}
		} while (did_cut);
	}
	
	bool outside_loop = false;
	if (trimmed.size() <= 2) {
		outside_loop = true;
	} else {
		//find upper rightmost:
		unsigned int test = 0;
		for (unsigned int i = 0; i < trimmed.size(); ++i) {
			if (points[trimmed[test]].x < points[trimmed[i]].x) {
				test = i;
			} else if (points[trimmed[test]].x == points[trimmed[i]].x) {
				if (points[trimmed[test]].y < points[trimmed[i]].y) {
					test = i;
				}
			}
		}
		//trimmed shouldn't have any fold-overs:
		assert(trimmed[(test+1)%loop.size()] != trimmed[(test+loop.size()-1)%loop.size()]);
		//check if boundary is counterclockwise:
		CClockwise cmp(points, loop[test]);
		if (cmp(loop[(test+1)%loop.size()], loop[(test+loop.size()-1)%loop.size()])) {
			outside_loop = true;
		}
	}

	if (!outside_loop) {
		//it's an inside loop, so it gets a new face by definition.
		loop_faces.push_back(face_loops.size());
		face_loops.push_back(vector< unsigned int >(1, loops.size()-1));
		assert(loop_faces.size() == loops.size());
		return true;
	}

	//it's an outside loop -- find the containing loop.
	Vector2i p = points[loop[0]];
	int64_t best_num = 1;
	int64_t best_den = 0;
	unsigned int best_face = 0;
	//find closest edge crossing (downward) the ray to the right of 'p':
	for (unsigned int l = 0; l + 1 < loops.size(); ++l) {
		for (unsigned int i = 0; i < loops[l].size(); ++i) {
			Vector2i a = points[loops[l][i]];
			Vector2i b = points[loops[l][(i+1)%loops[l].size()]];
			if (a.y >= p.y && b.y < p.y) {
				int64_t on_den = (a.y - b.y);
				int64_t on_num = (a.x - b.x) * (p.y - b.y) + on_den * (b.x - p.x);
				assert(on_den > 0);
				assert(on_num != 0);
				if (on_num > 0) {
					if (on_num * best_den < best_num * on_den) {
						best_num = on_num;
						best_den = on_den;
						best_face = loop_faces[l];
					}
				}
			}
		}
	}

	assert(best_face < face_loops.size());
	loop_faces.push_back(best_face);
	face_loops[best_face].push_back(loops.size()-1);


	assert(loop_faces.size() == loops.size());
	return true;
}

void PlanarMap::do_face_counts() {
	if (face_loops.empty()) return;

	unordered_map< Vector2ui, int, HashVector2ui > edge_to_count;
	assert(edge_counts.size() == edges.size());
	for (vector< Vector2ui >::iterator e = edges.begin(); e != edges.end(); ++e) {
		assert(!edge_to_count.count(*e));
		edge_to_count.insert(make_pair(*e, edge_counts[e - edges.begin()]));
		Vector2ui rev = make_vector(e->c[1], e->c[0]);
		assert(!edge_to_count.count(rev));
		edge_to_count.insert(make_pair(rev,-edge_counts[e - edges.begin()]));
	}

	unordered_map< Vector2ui, unsigned int, HashVector2ui > edge_to_face;
	assert(loop_faces.size() == loops.size());
	for (unsigned int l = 0; l < loops.size(); ++l) {
		for (unsigned int i = 0; i < loops[l].size(); ++i) {
			Vector2ui edge = make_vector(loops[l][i], loops[l][(i+1)%loops[l].size()]);
			assert(!edge_to_face.count(edge));
			edge_to_face.insert(make_pair(edge, loop_faces[l]));
		}
	}

	face_counts.resize(face_loops.size(), 0);
	vector< bool > visited(face_loops.size(), false);

	vector< unsigned int > to_expand;
	face_counts[0] = 0;
	visited[0] = true;
	to_expand.push_back(0);
	while (!to_expand.empty()) {
		unsigned int face = to_expand.back();
		to_expand.pop_back();
		assert(face < face_loops.size());
		assert(visited[face]);
		for (vector< unsigned int >::iterator li = face_loops[face].begin(); li != face_loops[face].end(); ++li) {
			vector< unsigned int > const &loop = loops[*li];
			for (unsigned int i = 0; i < loop.size(); ++i) {
				Vector2ui e = make_vector(loop[i], loop[(i+1)%loop.size()]);
				int new_count = face_counts[face];
				assert(edge_to_count.count(e));
				new_count += edge_to_count[e];
				std::swap(e.c[0], e.c[1]);
				assert(edge_to_face.count(e));
				unsigned int next_face = edge_to_face[e];
				if (visited[next_face]) {
					if (face_counts[next_face] != new_count) {
						LOG_INFO("Face count on " << next_face << " is " << face_counts[next_face] << ", but expected " << new_count);
					}
					//assert(face_counts[next_face] == new_count);
				} else {
					visited[next_face] = true;
					face_counts[next_face] = new_count;
					to_expand.push_back(next_face);
				}
			}
		}
	}

	//DEBUG: make sure we've visited everything
	for (vector< bool >::iterator v = visited.begin(); v != visited.end(); ++v) {
		assert(*v);
	}
}

bool PlanarMap::triangulate_face() {
	if (face_triangles.size() >= face_counts.size()) return false;
	unsigned int face_ind = face_triangles.size();
	if (fill_rule == EvenOdd) {
		if (face_counts[face_ind] % 2 == 0) {
			face_triangles.push_back(vector< Vector3ui >());
			return true;
		}
	} else {
		assert(fill_rule == NonZero);
		if (face_counts[face_ind] == 0) {
			face_triangles.push_back(vector< Vector3ui >());
			return true;
		}
	}
	//can't fill what's not there:
	if (face_loops[face_ind].empty()) {
		face_triangles.push_back(vector< Vector3ui >());
		return true;
	}
	//face has clockwise border [0] with counter-clockwise holes [1, ...]
	// ... or, at least, one hopes this is true ...
	//TODO: check loop directions.

	//record all the lines that new splits can't cross:
	vector< Vector2ui > lines;
	for (vector< unsigned int >::const_iterator li = face_loops[face_ind].begin(); li != face_loops[face_ind].end(); ++li) {
		for (unsigned int i = 0; i < loops[*li].size(); ++i) {
			lines.push_back(make_vector(loops[*li][i], loops[*li][(i+1)%loops[*li].size()]));
		}
	}

	//loop is the big outer loop:
	vector< unsigned int > loop = loops[face_loops[face_ind][0]];
	//fold in each hole:
	for (vector< unsigned int >::const_iterator hi = face_loops[face_ind].begin() + 1; hi != face_loops[face_ind].end(); ++hi) {
		vector< unsigned int > hole = loops[*hi];
		if (hole.size() < 3) continue; //ignore linear holes.
		{ //find the upper rightmost to connect out.
			unsigned int rightmost = 0;
			for (unsigned int i = 1; i < hole.size(); ++i) {
				if (points[hole[i]].x > points[hole[rightmost]].x) {
					rightmost = i;
				} else if (points[hole[i]].x == points[hole[rightmost]].x) {
					if (points[hole[i]].y > points[hole[rightmost]].y) {
						rightmost = i;
					}
				}
			}
			assert(rightmost < hole.size());
			size_t old_size = hole.size();
			//rotate hole so rightmost is at index 0:
			hole.insert(hole.end(), hole.begin(), hole.begin() + rightmost);
			hole.erase(hole.begin(), hole.begin() + rightmost);
			assert(hole.size() == old_size);
		}
		//find a visible vert in 'loop':
		Vector2i p_prev = points[hole.back()];
		Vector2i p = points[hole[0]];
		Vector2i p_next = points[hole[1]];
		//Since 'p' is upper rightmost it *must* be reflex.
		assert(line_side(p_next, p_prev, p) == 1);
		vector< unsigned int > visible_inds;
		for (unsigned int i = 0; i < loop.size(); ++i) {
			Vector2i v = points[loop[i]];
			//if v can't be seen because of p's incident faces, skip it
			if (line_side(v, p_prev, p) >= 0 && line_side(v, p, p_next) >= 0) {
				continue;
			}
			Vector2i v_prev = points[loop[(i+loop.size()-1)%loop.size()]];
			Vector2i v_next = points[loop[(i+1)%loop.size()]];
			//similarly, if p can't be seen because of v' incident faces, skip:
			bool v_reflex = (line_side(v_next, v_prev, v) == 1);
			if (v_reflex) {
				if (line_side(p, v_prev, v) >= 0 && line_side(p, v, v_next) >= 0) {
					continue;
				}
			} else {
				if (line_side(p, v_prev, v) >= 0 || line_side(p, v, v_next) >= 0) {
					continue;
				}
			}
			//now go ahead and check all the myriad other lines:
			bool good = true;
			for (vector< Vector2ui >::const_iterator line = lines.begin(); line != lines.end(); ++line) {
				Vector2i a = points[line->c[0]];
				Vector2i b = points[line->c[1]];
				if (a == p || a == v || b == p || b == v) continue;
				if (point_on_line(a, p, v) || point_on_line(b, p, v)) {
					good = false;
					break;
				}
				//the only problem here is if points lie on opposite sides;
				// lying on same side (or on the line) is okay.
				// thus, the only bad case is 1/-1
				assert((1 ^ -1) == -2);
				if ((line_side(p, a, b) ^ line_side(v, a, b)) != (1 ^ -1)) continue;
				if ((line_side(a, p, v) ^ line_side(b, p, v)) != (1 ^ -1)) continue;
				//they intersect, oh well.
				good = false;
				break;
			}
			if (good) {
				visible_inds.push_back(i);
			}
		}
		assert(!visible_inds.empty());
		{
			unsigned int splice_ind = visible_inds[0];
			hole.push_back(hole[0]);
			hole.push_back(loop[splice_ind]);
			loop.insert(loop.begin() + splice_ind + 1, hole.begin(), hole.end());
			lines.push_back(make_vector(loop[splice_ind], hole[0]));
		}
	}

	face_triangles.push_back(vector< Vector3ui >());
	//LOG_INFO(" ---- " << loop.size() << " ---- ");
	peel_loop(loop, face_triangles.back());
	return true;
}

//peel triangles out of a loop (loop oriented clockwise)
void PlanarMap::peel_loop(vector< unsigned int > const &loop, vector< Vector3ui > &tris) {
	if (loop.size() < 3) return;
	vector< unsigned int > reflex;
	vector< unsigned int > not_reflex;
	for (unsigned int i = 0; i < loop.size(); ++i) {
		Vector2i prev = points[loop[(i + loop.size() - 1) % loop.size()]];
		Vector2i cur = points[loop[i]];
		Vector2i next = points[loop[(i + 1) % loop.size()]];
		if (line_side(cur, prev, next) == 1) {
			not_reflex.push_back(i);
		} else {
			reflex.push_back(i);
		}
	}
	if (not_reflex.empty()) {
		LOG_WARNING("Non-clockwise loop of size " << loop.size() << ".");
		return;
	}
	unsigned int v = not_reflex[mt_rand->randInt(not_reflex.size()-1)];
	unsigned int close = -1U;
	{ //find possible other vert 'close' which is poking into the triangle at 'v':
		int64_t close_amt = -1;
		Vector2i prev = points[loop[(v + loop.size() - 1) % loop.size()]];
		Vector2i cur = points[loop[v]];
		Vector2i next = points[loop[(v + 1) % loop.size()]];
		for (vector< unsigned int >::iterator r = reflex.begin(); r != reflex.end(); ++r) {
			unsigned int i = *r;
			Vector2i p = points[loop[i]];
			if (line_side(p, cur, prev) != 1) continue;
			if (line_side(p, cur, next) != -1) continue;
			if (i == v || i + 1 == v || i == v + 1) {
				LOG_ERROR("Failure: got " << i << " when v was " << v << " linesides are: " << line_side(p, cur, prev) << " and " << line_side(p, cur, next) << ".");
				assert(!(i == v || i + 1 == v || i == v + 1)); //these should fail line-side
			}
			int64_t amt = make_vector< int64_t >(p - prev) * make_vector< int64_t >(perpendicular(next - prev));
			if (amt > close_amt) {
				close_amt = amt;
				close = i;
			}
		}
	} //end of find close...
	if (close == -1U) {
		//great, can just take triangle at 'v':
		Vector3ui tri = make_vector(loop[v], loop[(v+loop.size()-1)%loop.size()], loop[(v+1)%loop.size()]);
		int side = line_side(points[tri.c[1]], points[tri.c[0]], points[tri.c[2]]);
		if (side == -1) {
			tris.push_back(tri);
			//LOG_INFO(prefix << "tri " << tri);
		} else if (side != 0) {
			LOG_WARNING("Loop peeled to non-clockwise triangle.");
		} //side == 0 --> zero-volume triangle --> some sort of spur off of loop

		//and trim out 'v' from loop:
		vector< unsigned int > new_loop = loop;
		new_loop.erase(new_loop.begin() + v);

		//LOG_INFO(prefix << "split " << loop[v]);
		peel_loop(new_loop, tris /*, prefix + " "*/);
	} else {
		//LOG_INFO(prefix << "split " << loop[v] << "-" << loop[close]);
		//need to split between 'v' and 'close'...
		if (v > close) std::swap(v, close);
		//loop_a will be loop[v] ... loop[close]:
		vector< unsigned int > loop_a(loop.begin() + v, loop.begin() + close + 1);
		//loop_b will be loop[0] .. loop[v] loop[close] ... loop[N]
		vector< unsigned int > loop_b(loop.begin(), loop.begin() + v + 1);
		loop_b.insert(loop_b.end(), loop.begin() + close, loop.end());
		assert(loop_a.size() + loop_b.size() == loop.size() + 2);
		peel_loop(loop_a, tris /*, prefix + " "*/);
		peel_loop(loop_b, tris /*, prefix + " "*/);
	}
}
