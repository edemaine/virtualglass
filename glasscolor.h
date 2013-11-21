#ifndef GLASSCOLOR_H
#define GLASSCOLOR_H

#include <stack>
#include <string>

#include "primitives.h"

using std::string;
using std::stack;

class GlassColor 
{
	friend class UndoRedo;

	public:
		GlassColor();
		GlassColor(Color __color, string __shortName, string __longName);

		const Color color() const;
		void setColor(Color _color);
		const string shortName() const;
		void setShortName(string _name);
		const string longName() const;
		void setLongName(string _name);

		GlassColor* copy() const;

	private:
		Color _color;
		string _shortName;	
		string _longName;	
};

GlassColor* deep_copy(const GlassColor* _gc);

#endif


