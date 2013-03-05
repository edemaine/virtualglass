#include "SVG.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <deque>
#include <map>
#include <expat/lib/expat.h>

#define LOG_INFO( X ) std::cerr << X << std::endl

using std::string;
using std::vector;
using std::map;

namespace SVG {

namespace {

void wrap_start_element(void *, const XML_Char *_name, const XML_Char **_atts);
void wrap_end_element(void *, const XML_Char *_name);

class Parse;

class Error : public std::ostringstream {
public:
	Error(Parse *_parse) : parse(_parse) { }
	virtual ~Error();
	Parse *parse;
};

class Parse {
public:
Parse(std::istream &in, SVG &_into, PullPlan* myPullPlan) : into(_into) {
	pullPlan = myPullPlan;
	parser = XML_ParserCreate(NULL);
	XML_SetUserData(parser, this);
	XML_SetElementHandler(parser, &wrap_start_element, &wrap_end_element);

	while (!in.eof()) {
		const size_t BufferSize = 4096;
		void *buffer = XML_GetBuffer(parser, BufferSize);
		if (!buffer) {
			add_error("Ran out of memory allocating buffer");
			break;
		}

		in.read((char *)buffer, BufferSize);
		if (in.fail() && !in.eof()) {
			Error(this) << "Failed reading stream.";
			break;
		}
		if (XML_ParseBuffer(parser, in.gcount(), in.eof()) == XML_STATUS_ERROR) {
			Error(this) << "Error at " << XML_GetCurrentLineNumber(parser) << "." << XML_GetCurrentColumnNumber(parser) << ": "
			<< XML_ErrorString(XML_GetErrorCode(parser));
			break;
		}
	}

	XML_ParserFree(parser);
}

vector< Node * > stack;
vector< string > errors;
XML_Parser parser;
SVG &into;
PullPlan *pullPlan;

bool svg_length(string const &from, double &into) {
	std::istringstream str(from);
	double val = 0.0;
	string spec = "";
	if ((str >> val).fail() || ((str >> spec).fail() && !str.eof())) {
		Error(this) << "Bad SVG length '" << from << "'.";
	}
	while (!spec.empty() && isspace(spec[spec.size()-1])) {
		spec.erase(spec.size()-1, 1);
	}
	while (!spec.empty() && isspace(spec[0])) {
		spec.erase(0, 1);
	}
	if (spec == "" || spec == "px") {
		into = val;
		return true;
	} else {
		Error(this) << "Unsupported length modifier '" << spec << "'.";
		return false;
	}
}

bool svg_length(map< string, string > const &atts, string const &name, double &into) {
	map< string, string >::const_iterator f = atts.find(name);
	if (f == atts.end()) return false;
	return svg_length(f->second, into);
}

static string grab_nonwhitespace(string &s) {
	for (size_t i = 0; i < s.size(); ++i) {
		if (isspace(s[i])) {
			string ret = s.substr(0,i);
			s = s.substr(i);
			return ret;
		}
	}
	string ret = s;
	s = "";
	return ret;
}

static bool ischar(const char *set, char s) {
	for (const char *c = set; *c != '\0' ; ++c) {
		if (s == *c) {
			return true;
		}
	}
	return false;
}


static string grab_nonchar(const char *set, string &s) {
	for (size_t i = 0; i < s.size(); ++i) {
		if (ischar(set, s[i])) {
			string ret = s.substr(0,i);
			s = s.substr(i);
			return ret;
		}
	}
	string ret = s;
	s = "";
	return ret;
}

static uint32_t grab_hex(string &s) {
	uint32_t ret = 0;
	for (size_t i = 0; i < s.size(); ++i) {
		if (!isxdigit(s[i])) {
			s.erase(0, i);
			return ret;
		}
		char d = s[i];
		ret *= 16;
		if ('0' <= d && d <= '9') {
			ret += (d - '0');
		} else if ('A' <= d && d <= 'F') {
			ret += (d - 'A') + 10;
		} else if ('a' <= d && d <= 'f') {
			ret += (d - 'a') + 10;
		} else {
			assert(0);
		}
	}
	s = "";
	return ret;
}


static size_t eat_whitespace(string &s) {
	for (size_t i = 0; i < s.size(); ++i) {
		if (!isspace(s[i])) {
			s.erase(0,i);
			return i;
		}
	}
	return 0;
}

//trim whitespace from the right of a string.
static size_t trim_whitespace(string &s) {
	for (size_t i = s.size() - 1; i < s.size(); --i) {
		if (!isspace(s[i])) {
			s.erase(i+1, s.size()-1-i);
			return i;
		}
	}
	return 0;
}


static bool eat_char(const char *set, string &s) {
	if (s.empty()) return false;
	if (ischar(set, s[0])) {
		s.erase(0,1);
		return true;
	}
	return false;
}

static size_t eat_chars(const char *set, string &s) {
	size_t ret = 0;
	while (eat_char(set, s)) {
		++ret;
	}
	return ret;
}

static bool isnumber(std::string s) {
	//number is:
	//whitespace [+-]? [0-9]* .? [0-9]* (e[+-][0-9]+)? whitespace
	eat_whitespace(s);
	eat_char("+-",s);
	size_t digits = 0;
	digits += eat_chars("0123456789",s);
	eat_char(".", s);
	digits += eat_chars("0123456789",s);
	if (digits == 0) {
		return false;
	}
	if (eat_char("eE", s)) {
		eat_char("+-", s);
		if (eat_chars("0123456789",s) == 0) {
			return false;
		}
	}
	eat_whitespace(s);
	return s.empty();
}

//return css color in [0,1] RGB space
static bool css_color(string const &s, Vector4f &col) {
	if (s.empty()) return false;
	if (s == "none") {
		col = make_vector(0.0f, 0.0f, 0.0f, 0.0f);
		return true;
	}
	//#hex #hexhex syntax:
	if (s[0] == '#') {
		string r = "";
		string g = "";
		string b = "";
		if (s.size() == 4) {
			r = s.substr(1,1) + s.substr(1,1);
			g = s.substr(2,1) + s.substr(2,1);
			b = s.substr(3,1) + s.substr(3,1);
		} else if (s.size() == 7) {
			r = s.substr(1,2);
			g = s.substr(3,2);
			b = s.substr(5,2);
		} else {
			return false;
		}
		Vector3i vals = make_vector(0, 0, 0);
		vals[0] = grab_hex(r);
		vals[1] = grab_hex(g);
		vals[2] = grab_hex(b);
		if (r.empty() && g.empty() && b.empty()) {
			for (unsigned int i = 0; i < 3; ++i) {
				col[i] = vals[i] / 255.0f;
			}
			col[3] = 1.0f;
			return true;
		}
		return false;
	}

	//rgb( ) syntax:
	Vector3i vals = make_vector(0, 0, 0);
	if (sscanf(s.c_str(), "rgb( %d , %d , %d )", &(vals[0]), &(vals[1]), &(vals[2])) == 3) {
		for (unsigned int i = 0; i < 3; ++i) {
			col[i] = vals[i] / 255.0f;
		}
		col[3] = 1.0f;
		return true;
	}
	if (sscanf(s.c_str(), "rgb( %d %% , %d %% , %d %% )", &(vals[0]), &(vals[1]), &(vals[2])) == 3) {
		for (unsigned int i = 0; i < 3; ++i) {
			col[i] = vals[i] / 100.0f;
		}
		col[3] = 1.0f;
		return true;
	}

	return false;
}

void parse_paint(string const &name, string const &value, Paint &into) {
	if (value.empty()) {
		Error(this) << "Empty paint for " << name << ".";
		return;
	}
	Vector4f col = make_vector(0.0f, 0.0f, 0.0f, 0.0f);
	if (!css_color(value, col)) {
		Error(this) << "Bad CSS color '" << value << "' for " << name << ".";
		return;
	}
	into.color = col;
}

void parse_opacity(string const &name, string const &value, Paint &into) {
	std::istringstream str(value);
	float val = 0.0;
	if (!(str >> val && str.eof())) {
		Error(this) << "Bad opacity (or trailing garbage) '" << value << "' for " << name << ".";
		return;
	}
	into.opacity = val;
}

void start_element(string const &name, map< string, string > &atts) {
	if (stack.empty()) {
		stack.push_back(&into.root);
		if (name == "svg") {
			//great!
			if (!svg_length(atts, "width", into.page.x)) {
				add_error("svg tag missing width");
			}
			if (!svg_length(atts, "height", into.page.y)) {
				add_error("svg tag missing height");
			}
		} else {
			LOG_INFO("Ignoring tag '" << name << "' outside <svg>.");
		}
	} else {
		stack.back()->children.push_back(Node());
		stack.back()->children.back().copy_style(*stack.back());
		stack.push_back(&(stack.back()->children.back()));
	}
	if (stack.empty()) return;
	Node &node = *stack.back();
	//----------------------------
	//Handle transform:
	if (atts.count("transform")) {
		string val = atts["transform"];
		Matrix mat = identity_matrix< double, 2, 3 >();
		if (val.substr(0,6) == "matrix") {
			if (sscanf(val.c_str(), "matrix ( %lf , %lf , %lf , %lf , %lf , %lf )", &mat.c[0], &mat.c[3], &mat.c[1], &mat.c[4], &mat.c[2], &mat.c[5]) != 6) {
				Error(this) << "Error parsing matrix command '" << val << "'";
				XML_StopParser(parser, 0);
				return;
			}
		} else if (val.substr(0,9) == "translate") {
			if (sscanf(val.c_str(), "translate ( %lf , %lf )", &mat.c[2], &mat.c[5]) != 2) {
				Error(this) << "Error parsing translate command '" << val << "'";
				XML_StopParser(parser, 0);
				return;
			}
		} else {
			Error(this) << "Unknown transform '" << val << "'.";
			XML_StopParser(parser, 0);
			return;
		}
		node.transform = mat;
	}
	//----------------------------
	//Handle style:
	if (atts.count("style")) {
		map< string, string > props;
		{ //parse style into "property : value ;" pairs
			string style = atts["style"];
			while (!style.empty()) {
				eat_whitespace(style);
				string prop = grab_nonchar(":", style);
				trim_whitespace(prop);
				if (!eat_char(":", style)) {
					Error(this) << "No colon after '" << prop << "' while parsing style.";
					break;
				}
				eat_whitespace(style);
				string value = grab_nonchar(";", style);
				trim_whitespace(value);
				eat_char(";", style);
				/* Not an error, as last style entry doesn't need semicolon...
				if (!eat_char(";", style) && !style.empty()) {
					Error(this) << "No semicolon after value '" << value << "' while parsing style.";
					break;
				}
				*/
				if (props.count(prop)) {
					props[prop] = value;
				} else {
					props.insert(make_pair(prop, value));
				}
				//DEBUG:
				//std::cout << "Style -- " << prop << " => " << value << std::endl;
			}
		}
		//Interpret props:
		if (props.count("fill")) {
			parse_paint("fill", props["fill"], node.fill_paint);
		}
		if (props.count("fill-opacity")) {
			parse_opacity("fill", props["fill-opacity"], node.fill_paint);
		}
		if (props.count("stroke")) {
			parse_paint("stroke", props["stroke"], node.stroke_paint);
		}
		if (props.count("stroke-opacity")) {
			parse_opacity("stroke", props["stroke-opacity"], node.stroke_paint);
		}
		if (props.count("stroke-width")) {
			double width = 0.0;
			if (!svg_length(props["stroke-width"], width)) {
				Error(this) << "Bad stroke width '" << props["stroke-width"] << "'";
			} else {
				node.stroke_width = width;
			}
		}
		if (props.count("fill-rule")) {
			if (props["fill-rule"] == "evenodd") {
				node.fill_rule = Node::FILL_EVENODD;
			} else if (props["fill-rule"] == "nonzero") {
				node.fill_rule = Node::FILL_NONZERO;
			} else {
				Error(this) << "Unknown fill rule '" << props["fill-rule"] << "'";
			}
		}
	}
	//----------------------------
	//tags
	if (atts.count("inkscape:label")) {
		string label = atts["inkscape:label"];
		if (label.substr(0,4) == "tag-") {
			node.tag = label.substr(4);
		}
	}
	//----------------------------
	//Parse out a command list:
	std::deque< string > commands;
	//read command list if it's a path
	if (name == "path") {
		if (!atts.count("d")) {
			Error(this) << "Path without 'd'";
			XML_StopParser(parser, 0);
			return;
		}
		string d = atts["d"];
		for (unsigned int i = 0; i < d.size(); ++i) {
			char c = d[i];
			if (isspace(c) || c == ',') {
				if (!commands.empty() && commands.back() != "") {
					commands.push_back("");
				}
				continue;
			}
			if (commands.empty()) {
				commands.push_back("");
			}
			commands.back() += c;
		}
		if (!commands.empty() && commands.back() == "") {
			commands.pop_back();
		}
	} else if (name == "rect") {
		double x,y,width,height;
		if (svg_length(atts, "x", x) && svg_length(atts, "y", y) && svg_length(atts, "width", width) 
			&& svg_length(atts, "height", height)) {
			if (width == height) {

			// Grab page size (right now we assume it's square because it will error later otherwise).
			float pageSize = (float) into.page.c[0];

			// Convert coordinates from 0, 0 in upper left to 0,0 in center and scale
			Point2D p = make_vector((float) ((x +width/2-pageSize/2)/ pageSize* sqrt(2.0)),
					      (float) (( y +width/2 - pageSize/2)/ pageSize*sqrt(2.0)));
			// Scale radius
			float diameter = width/ pageSize * sqrt(2);

			// Add square to pullPlan
			pullPlan->subs.insert(pullPlan->subs.begin(),
			    SubpullTemplate(new PullPlan(PullTemplate::BASE_SQUARE), SQUARE_SHAPE, p, diameter));
			} else {
				node.moveto(make_vector(x,y));
				node.lineto(make_vector(x+width,y));
				node.lineto(make_vector(x+width,y+height));
				node.lineto(make_vector(x,y+height));
				node.closepath();
			}
			} else {
				Error(this) << "rect without x,y,width, or height.";
			}
			} else if (name == "circle") {

				double cx, cy, r;
				if (svg_length(atts, "cx", cx) && svg_length(atts, "cy", cy) && svg_length(atts, "r", r)) {

				// Grab page size (right now we assume it's square because it will error later otherwise).
				float pageSize = (float) into.page.c[0];

				// Convert coordinates from 0, 0 in upper left to 0,0 in center and scale
				Point2D p = make_vector((float) ((cx -pageSize/2)/ pageSize* sqrt(2.0)),
						  (float) (( cy - pageSize/2)/ pageSize*sqrt(2.0)));
				// Scale radius
				float diameter = 2*r/ pageSize * sqrt(2);

				// Add circle to pullPlan
				pullPlan->subs.insert(pullPlan->subs.begin(),
				SubpullTemplate(new PullPlan(PullTemplate::BASE_CIRCLE), CIRCLE_SHAPE, p, diameter));

			} else {
				Error(this) << "circle without cx, cy, or r.";
			}
	}

	{ //translate command list
		Vector2d subpath_start = make_vector(0.0, 0.0);
		Vector2d prev = make_vector(0.0, 0.0);
		while (!commands.empty()) {
			string c = commands.front();
			commands.pop_front();
			//slerp up subsequent numbers into coords:
			vector< double > coords;
			while (!commands.empty() && isnumber(commands.front())) {
				coords.push_back(atof(commands.front().c_str()));
				commands.pop_front();
			}
			assert(c != ""); //wouldn't be in command list otherwise.
			if (c == "m" || c == "M") {
				//moveto.
				if (coords.size() < 2 || (coords.size() % 2) != 0) {
					Error(this) << "Moveto with " << coords.size() << " coords.";
					XML_StopParser(parser, 0);
					return;
				}
				if (c == "m") {
					coords[0] += prev.x;
					coords[1] += prev.y;
				}
				//moveto starts a new subpath:
				prev = subpath_start = make_vector(coords[0], coords[1]);
				node.moveto(prev);
				coords.erase(coords.begin(), coords.begin() + 2);
				//any extra coords are implicit linetos:
				if (!coords.empty()) {
					if (c == "m") {
						c = "l";
					} else {
						c = "L";
					}
				} else {
					c = "";
				}
			}
			if (c == "") {
				//special 'skip this' logic for implicit lineto handling.
			} else if (c == "l" || c == "L") {
				if (coords.size() < 2 || (coords.size() % 2) != 0) {
					Error(this) << "lineto with " << coords.size() << " coords.";
					XML_StopParser(parser, 0);
					return;
				}
				for (size_t i = 0; i + 1 < coords.size(); i += 2) {
					if (c == "l") {
						coords[i] += prev.x;
						coords[i+1] += prev.y;
					}
					prev = make_vector(coords[i], coords[i+1]);
					node.lineto(prev);
				}
				coords.clear();
			} else if (c == "h" || c == "H") {
				if (coords.size() < 1) {
					Error(this) << "hlineto with " << coords.size() << " coords.";
					XML_StopParser(parser, 0);
					return;
				}
				for (size_t i = 0; i < coords.size(); ++i) {
					if (c == "h") {
						coords[i] += prev.x;
					}
					prev.x = coords[i];
					node.lineto(prev);
				}
				coords.clear();
			} else if (c == "v" || c == "V") {
				if (coords.size() < 1) {
					Error(this) << "vlineto with " << coords.size() << " coords.";
					XML_StopParser(parser, 0);
					return;
				}
				for (size_t i = 0; i < coords.size(); ++i) {
					if (c == "v") {
						coords[i] += prev.y;
					}
					prev.y = coords[i];
					node.lineto(prev);
				}
				coords.clear();
			} else if (c == "a" || c == "A") {
				if (coords.size() < 7 || (coords.size() % 7) != 0) {
					Error(this) << "arcto with " << coords.size() << " coords.";
					XML_StopParser(parser, 0);
					return;
				}
				for (size_t i = 0; i + 6 < coords.size(); i += 7) {
					if (c == "a") {
						coords[i+5] += prev.x;
						coords[i+6] += prev.y;
					}
					double ang = coords[i+2] / 180.0 * M_PI;
					Vector2d x_axis = make_vector(cos(ang), sin(ang));
					Vector2d y_axis = perpendicular(x_axis);
					x_axis *= coords[i];
					y_axis *= coords[i+1];
					ArcInfo info(coords[i+3] != 0.0, coords[i+4] != 0.0);
					prev = make_vector(coords[i+5], coords[i+6]);
					node.arcto(x_axis, y_axis, info, prev);
				}
				coords.clear();
			} else if (c == "c" || c == "C") {
				if (coords.size() < 6 || (coords.size() % 6) != 0) {
					Error(this) << "curveto with " << coords.size() << " coords.";
					XML_StopParser(parser, 0);
					return;
				}
				for (size_t i = 0; i + 5 < coords.size(); i += 6) {
					if (c == "c") {
						coords[i] += prev.x;
						coords[i+1] += prev.y;
						coords[i+2] += prev.x;
						coords[i+3] += prev.y;
						coords[i+4] += prev.x;
						coords[i+5] += prev.y;
					}
					prev = make_vector(coords[i+4], coords[i+5]);
					node.curveto(make_vector(coords[i], coords[i+1]), make_vector(coords[i+2], coords[i+3]), make_vector(coords[i+4], coords[i+5]));
				}
				coords.clear();
			} else if (c == "z" || c == "Z") {
				if (coords.size() != 0) {
					Error(this) << "closepath with " << coords.size() << " coords.";
					XML_StopParser(parser, 0);
					return;
				}
				prev = subpath_start;
				node.closepath();
			} else {
				Error(this) << "Unknown command '" << c << "' in path.";
				XML_StopParser(parser, 0);
				return;
			}
		}
	}
}
void end_element(string const &/* unused name*/) {
	if (!stack.empty()) {
		if (stack.size() > 1) {
			assert(stack.back() == &(stack[stack.size() - 2]->children.back()));
			//prune empties...
			if (stack.back()->ops.empty() && stack.back()->children.empty() && stack.back()->tag == "") {
				stack[stack.size() - 2]->children.pop_back();
			}
		}
		stack.pop_back();
	}
}

void add_error(std::string err) {
	std::cerr << "Error parsing SVG: " << err << std::endl;
	errors.push_back(err);
}

};

Error::~Error() {
	assert(parse);
	parse->add_error(this->str());
}

void wrap_start_element(void *_ctx, const XML_Char *_name, const XML_Char **_atts) {
	Parse *ctx = reinterpret_cast< Parse * >(_ctx);
	string name = _name;
	map< string, string > atts;
	while (*_atts) {
		string att = *_atts;
		++_atts;
		assert(*_atts); //_atts should be name/value *pairs*
		string val = *_atts;
		++_atts;
		atts.insert(make_pair(att, val));
	}
	ctx->start_element(name, atts);
}

void wrap_end_element(void *_ctx, const XML_Char *_name) {
	Parse *ctx = reinterpret_cast< Parse * >(_ctx);
	ctx->end_element(_name);
}

//make arc from (start,end], adding points to 'into'.
void make_arc(double tol, Vector2d const &start, Vector2d const &end, Vector2d x_axis, Vector2d y_axis, ArcInfo const &info, vector< Vector2d > &into) {
	//Figure out transform into arc space:
	Vector2d world_to_arc_x;
	Vector2d world_to_arc_y;
	{
		double a = x_axis.x;
		double b = y_axis.x;
		double c = x_axis.y;
		double d = y_axis.y;
		double det = a * d - c * b;
		if (det == 0.0) {
			into.push_back(end);
			return;
		}
		double inv_det = 1.0 / det;
		world_to_arc_x.x =  d * inv_det;
		world_to_arc_x.y = -b * inv_det;
		world_to_arc_y.x = -c * inv_det;
		world_to_arc_y.y =  a * inv_det;
	}
	//Offset from world to arc:
	Vector2d world_to_arc_ofs = -0.5 * (start + end);

	Vector2d local_start = make_vector(world_to_arc_x * (start + world_to_arc_ofs), world_to_arc_y * (start + world_to_arc_ofs));
	Vector2d local_end = make_vector(world_to_arc_x * (end + world_to_arc_ofs), world_to_arc_y * (end + world_to_arc_ofs));

	//need centers of circle of radius 1 which contain local_start and local_end.
	double dis_sqr = 0.25 * length_squared(local_start - local_end);
	double remain = 1.0 - dis_sqr;
	double ofs = 0.0;
	double scale = 1.0;
	if (remain <= 0.0) {
		//exactly one circle.
		ofs = 0.0;
		scale = sqrt(dis_sqr); //need to scale up so points are tangent
	} else {
		//two circles, with +/- ofs:
		ofs = sqrt(remain);
	}
	Vector2d local_center = ((info.sweep_flag != info.large_arc_flag) ? ofs : -ofs) * normalize(perpendicular(local_end - local_start));
	double ang_start = atan2(local_start.y - local_center.y, local_start.x - local_center.x);
	double ang_end = atan2(local_end.y - local_center.y, local_end.x - local_center.x);
	if (!info.sweep_flag) {
		std::swap(ang_start, ang_end);
	}
	if (ang_end < ang_start) {
		ang_end += M_PI * 2.0;
	}
	Vector2d center = local_center.x * x_axis + local_center.y * y_axis - world_to_arc_ofs;

	//scale up axis if needed to meet arc obligations:
	x_axis *= scale;
	y_axis *= scale;

	//Want that lines are within 'tol' of actual arc.
	//In a circle, this would mean that:
	//r - r * sin(step) == tol
	// sin(step) == 1 - tol / r
	double rad_bound = length(x_axis) + length(y_axis);
	double max_step = 1.0 - tol / rad_bound;
	if (max_step <= M_SQRT1_2) {
		//can just take huge steps.
		max_step = M_PI_4;
	} else {
		max_step = acos(max_step);
	}
	unsigned int steps = (ang_end - ang_start) / max_step;
	steps += 1;
	int step = info.sweep_flag ? 1 : -1;
	unsigned int first = info.sweep_flag ? 0 : (steps - 1);
	for (unsigned int a = first; a < steps; a += step) {
		double ang = ang_start + (ang_end - ang_start) * ((a + 0.5) / double(steps));
		into.push_back(cos(ang) * x_axis + sin(ang) * y_axis + center);
	}
	into.push_back(end);

}

void make_curve(double tol, Vector2d const &c0, Vector2d const &c1, Vector2d const &c2, Vector2d const &c3, vector< Vector2d > &into) {
	double len_bound = length(c1 - c0) + length(c2 - c1) + length(c3 - c2);
	if (len_bound < tol) {
		into.push_back(c3);
		return;
	}
	unsigned int steps = len_bound / tol;
	steps += 1;
	for (unsigned int s = 0; s < steps; ++s) {
		double t = (s + 0.5) / double(steps);
		Vector2d b0 = c0 + (c1 - c0) * t;
		Vector2d b1 = c1 + (c2 - c1) * t;
		Vector2d b2 = c2 + (c3 - c2) * t;
		Vector2d a0 = b0 + (b1 - b0) * t;
		Vector2d a1 = b1 + (b2 - b1) * t;
		into.push_back(a0 + (a1 - a0) * t);
	}
	into.push_back(c3);
}
} //end anon namespace


void Node::execute(Matrix const &xform, double tol, vector< vector< Vector2d > > &out) const {
	//transform coords:
	vector< Vector2d > xc(coords.size());
	for (size_t i = 0; i < xc.size(); ++i) {
		xc[i] = make_vector(xform * make_matrix(make_vector(coords[i], 1.0)));
	}
	Vector2d origin = xform * make_vector(0.0, 0.0, 1.0);
	//Actually run through ops:
	vector< Vector2d >::const_iterator c = xc.begin();
	vector< ArcInfo >::const_iterator ai = arc_infos.begin();
	vector< Vector2d > subpath;
	Vector2d prev = make_vector(xform * make_matrix(make_vector(0.0, 0.0, 1.0)));
	for (vector< char >::const_iterator op = ops.begin(); op != ops.end(); ++op) {
		if (*op == OP_MOVETO) {
			if (!subpath.empty()) {
				out.push_back(subpath);
				subpath.clear();
			}
			assert(c != xc.end());
			subpath.push_back(*c);
			prev = *c;
			++c;
		} else if (*op == OP_LINETO) {
			if (subpath.empty()) {
				subpath.push_back(prev);
			}
			assert(c != xc.end());
			subpath.push_back(*c);
			prev = *c;
			++c;
		} else if (*op == OP_ARCTO) {
			if (subpath.empty()) {
				subpath.push_back(prev);
			}
			assert(c != xc.end());
			Vector2d x_axis = *c - origin;
			++c;
			assert(c != xc.end());
			Vector2d y_axis = *c - origin;
			++c;
			assert(c != xc.end());
			Vector2d end = *c;
			++c;
			assert(ai != arc_infos.end());
			ArcInfo info = *ai;
			++ai;
			Vector2d start = subpath.back();
			make_arc(tol, start, end, x_axis, y_axis, info, subpath);
			prev = end;
		} else if (*op == OP_CURVETO) {
			if (subpath.empty()) {
				subpath.push_back(prev);
			}
			assert(c != xc.end());
			Vector2d c1 = *c;
			++c;
			assert(c != xc.end());
			Vector2d c2 = *c;
			++c;
			assert(c != xc.end());
			Vector2d c3 = *c;
			++c;
			Vector2d c0 = subpath.back();
			make_curve(tol, c0, c1, c2, c3, subpath);
			prev = c3;
		} else if (*op == OP_CLOSEPATH) {
			if (!subpath.empty()) {
				subpath.push_back(subpath[0]);
				out.push_back(subpath);
				prev = subpath.back();
				subpath.clear();
			}
		} else {
			//not implemented.
			assert(0);
		}
	}
	if (!subpath.empty()) {
		out.push_back(subpath);
		subpath.clear();
	}
}

bool load_svg(std::string const &filename, SVG &into, PullPlan *pullPlan) {
	std::ifstream file(filename.c_str());
	Parse parse(file, into, pullPlan);
	return parse.errors.empty();
}

}
