
#ifndef UNDOREDO_H
#define UNDOREDO_H 
	
#include <string>

using std::string;

namespace UndoRedo
{
	void newDo(string event);
	const string current();
	void undo();
	bool canUndo();
	void redo();
	bool canRedo();
}

#endif

