
#include "glasscolor.h"


GlassColor :: GlassColor()
{
	Color c;
	c.r = c.g = c.b = 1.0;
	c.a = 0.0;
	color = c;
	name = "R-100";
	setDirtyBitColor();
}

GlassColor :: GlassColor(Color _color, QString _name)
{
	setColor(_color);
	setName(_name);
}

bool GlassColor :: getDirtyBitColor()
{
	return dirtyBit;
}

void GlassColor :: setDirtyBitColor(bool value)
{
	dirtyBit = value;
}

void GlassColor :: setColor(Color _color) 
{
	color = _color;
	this->setDirtyBitColor();
}

Color* GlassColor :: getColor()
{
	return &color;
}

void GlassColor :: setName(QString _name)
{
	name = _name;
}

QString* GlassColor :: getName()
{
	return &name;
}

GlassColor* GlassColor :: copy() const
{
	GlassColor* newGlassColor = new GlassColor();
	newGlassColor->color = color;
	newGlassColor->name = name;
	newGlassColor->setDirtyBitColor();
	return newGlassColor;
}

GlassColor* deep_copy(const GlassColor* _gc)
{
	// deep copy and copy are the same because of lack of subobjects 
	// Note: color and name aren't subobjects, they're unique to this glass color object
	return _gc->copy();
}
