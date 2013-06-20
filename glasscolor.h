#ifndef GLASSCOLOR_H
#define GLASSCOLOR_H

#include "primitives.h"
#include <string>

using std::string;

class GlassColor 
{
	public:
		GlassColor();
		GlassColor(Color __color, string __shortName, string __longName);

		Color color();
		void setColor(Color _color);

		string shortName();
		void setShortName(string _name);

		string longName();
		void setLongName(string _name);

		GlassColor* copy() const;

	private:
		Color _color;
		string _shortName;	
		string _longName;	
};

GlassColor* deep_copy(const GlassColor* _gc);


#endif


