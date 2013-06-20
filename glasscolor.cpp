
#include "glasscolor.h"


GlassColor :: GlassColor()
{
	Color c;
	c.r = c.g = c.b = 1.0;
	c.a = 0.0;
	_color = c;
	_shortName = "R-100"; 
	_longName = "R-100 Lead Crystal (Clear)";
}

GlassColor :: GlassColor(Color __color, string __shortName, string __longName)
{
	setColor(__color);
	setShortName(__shortName);
	setLongName(__longName);
}

void GlassColor :: setColor(Color __color) 
{
	_color = __color;
}

const Color GlassColor :: color() const
{
	return _color;
}

void GlassColor :: setShortName(string _name)
{
	_shortName = _name;
}

void GlassColor :: setLongName(string _name)
{
	_longName = _name;
}

const string GlassColor :: shortName() const
{
	return _shortName;
}

const string GlassColor :: longName() const
{
	return _longName;
}

GlassColor* GlassColor :: copy() const
{
	GlassColor* newGlassColor = new GlassColor();
	newGlassColor->_color = _color;
	newGlassColor->_shortName = _shortName;
	newGlassColor->_longName = _longName;
	return newGlassColor;
}

GlassColor* deep_copy(const GlassColor* _gc)
{
	// deep copy and copy are the same because of lack of subobjects 
	// Note: color and name aren't subobjects, they're unique to this glass color object
	return _gc->copy();
}
