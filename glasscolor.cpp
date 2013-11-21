
#include "glasscolor.h"


GlassColor :: GlassColor()
{
	Color c;
	c.r = c.g = c.b = 1.0;
	c.a = 0.0;
	this->_color = c;
	this->_shortName = "R-100"; 
	this->_longName = "R-100 Lead Crystal (Clear)";
}

GlassColor :: GlassColor(Color color, string shortName, string longName)
{
	setColor(color);
	setShortName(shortName);
	setLongName(longName);
}

void GlassColor :: setColor(Color color) 
{
	this->_color = color;
}

const Color GlassColor :: color() const
{
	return this->_color;
}

void GlassColor :: setShortName(string name)
{
	this->_shortName = name;
}

void GlassColor :: setLongName(string name)
{
	this->_longName = name;
}

const string GlassColor :: shortName() const
{
	return this->_shortName;
}

const string GlassColor :: longName() const
{
	return this->_longName;
}

GlassColor* GlassColor :: copy() const
{
	GlassColor* newGlassColor = new GlassColor();
	newGlassColor->_color = this->_color;
	newGlassColor->_shortName = this->_shortName;
	newGlassColor->_longName = this->_longName;
	return newGlassColor;
}

GlassColor* deep_copy(const GlassColor* _gc)
{
	// deep copy and copy are the same because of lack of subobjects 
	// Note: color and name aren't subobjects, they're unique to this glass color object
	return _gc->copy();
}




