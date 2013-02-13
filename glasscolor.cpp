
#include "glasscolor.h"


GlassColor :: GlassColor()
{
	Color c;
	c.r = c.g = c.b = 1.0;
	c.a = 0.0;
	color = c;
	shortName = "R-100"; 
	longName = "R-100 Lead Crystal (Clear)";
}

GlassColor :: GlassColor(Color _color, string _shortname, string _longname)
{
	setColor(_color);
	setShortName(_shortname);
	setLongName(_longname);
}

void GlassColor :: setColor(Color _color) 
{
	color = _color;
}

Color GlassColor :: getColor()
{
	return color;
}

void GlassColor :: setShortName(string _name)
{
	shortName = _name;
}

void GlassColor :: setLongName(string _name)
{
	longName = _name;
}

string GlassColor :: getShortName()
{
	return shortName;
}

string GlassColor :: getLongName()
{
	return longName;
}

GlassColor* GlassColor :: copy() const
{
	GlassColor* newGlassColor = new GlassColor();
	newGlassColor->color = color;
	newGlassColor->shortName = shortName;
	newGlassColor->longName = longName;
	return newGlassColor;
}

GlassColor* deep_copy(const GlassColor* _gc)
{
	// deep copy and copy are the same because of lack of subobjects 
	// Note: color and name aren't subobjects, they're unique to this glass color object
	return _gc->copy();
}
