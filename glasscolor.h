#ifndef GLASSCOLOR_H
#define GLASSCOLOR_H

#include "primitives.h"
#include <string>

using std::string;

class GlassColor 
{
	public:
		GlassColor();
		GlassColor(Color _color, string _shortName, string _longName);

		Color getColor();
		void setColor(Color _color);

		string getShortName();
		void setShortName(string _name);

		string getLongName();
		void setLongName(string _name);

		GlassColor* copy() const;

	private:
		Color color;
		string shortName;	
		string longName;	
};

GlassColor* deep_copy(const GlassColor* _gc);


#endif


