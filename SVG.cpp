/*
 * (unfinished) SVG parsing code from an (unfinished) game Jim has
 * been working on. This code is released into the public domain.
 */
#include "SVG.hpp"

#include <expat.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <deque>
#include <map>

#include <Basis/Error.hpp>

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
Parse(std::istream &in, SVG &_into) : into(_into) {
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

bool svg_length(map< string, string > const &atts, string const &name, double &into) {
	map< string, string >::const_iterator f = atts.find(name);
	if (f == atts.end()) return false;
	std::istringstream str(f->second);
	double val = 0.0;
	string spec = "";
	if ((str >> val).fail() || ((str >> spec).fail() && !str.eof())) {
		Error(this) << "Bad SVG length '" << f->second << "'.";
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

static size_t eat_whitespace(string &s) {
	for (size_t i = 0; i < s.size(); ++i) {
		if (!isspace(s[i])) {
			s.erase(0,i);
			return i;
		}
	}
	return 0;
}

static bool eat_char(const char *set, string &s) {
	if (s.empty()) return false;
	for (const char *c = set; *c != '\0' ; ++c) {
		if (s[0] == *c) {
			s.erase(0,1);
			return true;
		}
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
	//TODO
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
		if (svg_length(atts, "x", x) && svg_length(atts, "y", y) && svg_length(atts, "width", width) && svg_length(atts, "height", height)) {
			node.moveto(make_vector(x,y));
			node.lineto(make_vector(x+width,y));
			node.lineto(make_vector(x+width,y+height));
			node.lineto(make_vector(x,y+height));
			node.closepath();
		} else {
			Error(this) << "rect without x,y,width, or height.";
		}
	}
	//TODO: generate command list if it's a circle element
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
void end_element(string const &name) {
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

} //end anon namespace

void Node::execute(Matrix const &xform, double tol, vector< vector< Vector2d > > &out) const {
	//transform coords:
	vector< Vector2d > xc(coords.size());
	for (size_t i = 0; i < xc.size(); ++i) {
		xc[i] = make_vector(xform * make_matrix(make_vector(coords[i], 1.0)));
	}
	//Actually run through ops:
	vector< Vector2d >::const_iterator c = xc.begin();
	vector< Vector2d > subpath;
	Vector2d prev = make_vector(xform * make_matrix(make_vector(0.0, 0.0, 1.0)));
	for (vector< char >::const_iterator op = ops.begin(); op != ops.end(); ++op) {
		if (*op == 'M') {
			if (!subpath.empty()) {
				out.push_back(subpath);
				subpath.clear();
			}
			assert(c != xc.end());
			subpath.push_back(*c);
			prev = *c;
			++c;
		} else if (*op == 'L') {
			if (subpath.empty()) {
				subpath.push_back(prev);
			}
			assert(c != xc.end());
			subpath.push_back(*c);
			prev = *c;
			++c;
		} else if (*op == 'Z') {
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

bool load_svg(std::string const &filename, SVG &into) {
	std::ifstream file(filename.c_str());
	Parse parse(file, into);
	return parse.errors.empty();
}

}
