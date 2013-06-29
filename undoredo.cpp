
#include <stack>

#include "undoredo.h"

using std::stack;

namespace UndoRedo
{
	stack<string> undoStack;
	stack<string> redoStack;

	void newDo(string action)
	{
		undoStack.push(action);
		while (redoStack.size() > 0)
			redoStack.pop();
	}

	const string current()
	{
		return undoStack.top();
	}

	void undo()
	{
		if (canUndo())
		{
			redoStack.push(undoStack.top());
			undoStack.pop();
		}
	}

	void redo()
	{
		if (canRedo())
		{
			undoStack.push(redoStack.top());
			redoStack.pop();
		}
	}

	bool canRedo()
	{
		return redoStack.size() > 0;
	}

	bool canUndo()
	{
		return undoStack.size() > 0;
	}
}

