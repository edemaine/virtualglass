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
		GlassColor(Color __color, string __shortName, string __longName);

		const Color color() const;
		void setColor(Color _color);
		const string shortName() const;
		void setShortName(string _name);
		const string longName() const;
		void setLongName(string _name);

		GlassColor* copy() const;

		void undo();
		void redo();
		bool canUndo();
		bool canRedo();
		void saveState();			

	private:
		struct State
		{
			Color color;
			string shortName;	
			string longName;	
		};

		stack<struct State> undoStack;
		stack<struct State> redoStack;
		struct State state;	
};

GlassColor* deep_copy(const GlassColor* _gc);
void deep_delete(GlassColor* _gc);


#endif


