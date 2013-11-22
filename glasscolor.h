#ifndef GLASSCOLOR_H
#define GLASSCOLOR_H

#include <stack>
#include <string>

#include "primitives.h"

using std::string;
using std::stack;

class GlassColor 
{
	public:
		GlassColor();
		GlassColor(Color c, string s, string l);

		const Color color() const;
		void setColor(Color c);
		const string shortName() const;
		void setShortName(string s);
		const string longName() const;
		void setLongName(string s);

		GlassColor* copy() const;
		void set(GlassColor* c);

	private:
		Color color_;
		string shortName_;	
		string longName_;	
};

GlassColor* deep_copy(const GlassColor* _gc);

#endif


