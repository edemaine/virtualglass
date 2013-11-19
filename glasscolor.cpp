
#include "glasscolor.h"


GlassColor :: GlassColor()
{
	Color c;
	c.r = c.g = c.b = 1.0;
	c.a = 0.0;
	this->state.color = c;
	this->state.shortName = "R-100"; 
	this->state.longName = "R-100 Lead Crystal (Clear)";
	
	undoStack.push(this->state);
}

GlassColor :: GlassColor(Color color, string shortName, string longName)
{
	setColor(color);
	setShortName(shortName);
	setLongName(longName);
}

void GlassColor :: setColor(Color color) 
{
	this->state.color = color;
}

const Color GlassColor :: color() const
{
	return this->state.color;
}

void GlassColor :: setShortName(string name)
{
	this->state.shortName = name;
}

void GlassColor :: setLongName(string name)
{
	this->state.longName = name;
}

const string GlassColor :: shortName() const
{
	return this->state.shortName;
}

const string GlassColor :: longName() const
{
	return this->state.longName;
}

GlassColor* GlassColor :: copy() const
{
	GlassColor* newGlassColor = new GlassColor();
	newGlassColor->state = this->state;
	return newGlassColor;
}

void GlassColor :: undo()
{
	if (!canUndo())
		return;
	redoStack.push(undoStack.top());
	undoStack.pop();
	this->state = undoStack.top();
}

void GlassColor :: redo()
{
	if (!canRedo())
		return;
	undoStack.push(redoStack.top());
	redoStack.pop();
	this->state = undoStack.top();
}

bool GlassColor :: canUndo()
{
        return (undoStack.size() >= 2);
}

bool GlassColor :: canRedo()
{
        return (redoStack.size() > 0);
}

void GlassColor :: saveState()
{
	undoStack.push(this->state);
	while (redoStack.size() > 0)
		redoStack.pop();	
}


GlassColor* deep_copy(const GlassColor* _gc)
{
	// deep copy and copy are the same because of lack of subobjects 
	// Note: color and name aren't subobjects, they're unique to this glass color object
	return _gc->copy();
}




