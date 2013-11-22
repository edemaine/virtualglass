
#include "glasscolor.h"


GlassColor :: GlassColor()
{
	Color c;
	c.r = c.g = c.b = 1.0;
	c.a = 0.0;
	this->color_ = c;
	this->shortName_ = "R-100"; 
	this->longName_ = "R-100 Lead Crystal (Clear)";
}

GlassColor :: GlassColor(Color color, string shortName, string longName)
{
	setColor(color);
	setShortName(shortName);
	setLongName(longName);
}

void GlassColor :: set(GlassColor* c)
{
	this->color_ = c->color_;
	this->shortName_ = c->shortName_;
	this->longName_ = c->longName_;
}

void GlassColor :: setColor(Color color) 
{
	this->color_ = color;
}

const Color GlassColor :: color() const
{
	return this->color_;
}

void GlassColor :: setShortName(string name)
{
	this->shortName_ = name;
}

void GlassColor :: setLongName(string name)
{
	this->longName_ = name;
}

const string GlassColor :: shortName() const
{
	return this->shortName_;
}

const string GlassColor :: longName() const
{
	return this->longName_;
}

GlassColor* GlassColor :: copy() const
{
	GlassColor* newGlassColor = new GlassColor();
	newGlassColor->color_ = this->color_;
	newGlassColor->shortName_ = this->shortName_;
	newGlassColor->longName_ = this->longName_;
	return newGlassColor;
}

GlassColor* deep_copy(const GlassColor* _gc)
{
	// deep copy and copy are the same because of lack of subobjects 
	// Note: color and name aren't subobjects, they're unique to this glass color object
	return _gc->copy();
}




