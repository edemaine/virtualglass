
#include "globalundoredo.h"
#include "undoredo.h"

namespace GlobalUndoRedo
{
	UndoRedo* undoRedo = 0;

	void setMainWindow(MainWindow* mainWindow)
	{
		undoRedo = new UndoRedo(mainWindow);
	}	
	
	void undo()
	{
		if (undoRedo == 0)
			return;
		undoRedo->undo();
	}

	void redo()
	{
		if (undoRedo == 0)
			return;
		undoRedo->redo();
	}

	void addedGlassColor(GlassColor* gc, unsigned int index)
	{
		if (undoRedo == 0)
			return;
		undoRedo->addedGlassColor(gc, index);
	}

	void deletedGlassColor(GlassColor* gc, unsigned int index)
	{
		if (undoRedo == 0)
			return;
		undoRedo->deletedGlassColor(gc, index);
	}

	void modifiedGlassColor(GlassColor* gc)
	{
		if (undoRedo == 0)
			return;
		undoRedo->modifiedGlassColor(gc);
	}

	void movedGlassColor(unsigned int index, int direction)
	{
		if (undoRedo == 0)
			return;
		undoRedo->movedGlassColor(index, direction);
	}

	void addedCane(Cane* c, unsigned int index)
	{
		if (undoRedo == 0)
			return;
		undoRedo->addedCane(c, index);
	}

	void deletedCane(Cane* c, unsigned int index)
	{
		if (undoRedo == 0)
			return;
		undoRedo->deletedCane(c, index);
	}

	void modifiedCane(Cane* c)
	{
		if (undoRedo == 0)
			return;
		undoRedo->modifiedCane(c);
	}

	void movedCane(unsigned int index, int direction)
	{
		if (undoRedo == 0)
			return;
		undoRedo->movedCane(index, direction);
	}

	void addedPiece(Piece* p, unsigned int index)
	{
		if (undoRedo == 0)
			return;
		undoRedo->addedPiece(p, index);
	}

	void deletedPiece(Piece* p, unsigned int index)
	{
		if (undoRedo == 0)
			return;
		undoRedo->deletedPiece(p, index);
	}

	void modifiedPiece(Piece* p)
	{
		if (undoRedo == 0)
			return;
		undoRedo->modifiedPiece(p);
	}

	void movedPiece(unsigned int index, int direction)
	{
		if (undoRedo == 0)
			return;
		undoRedo->movedPiece(index, direction);
	}

	void noPriorUndo()
	{
		if (undoRedo == 0)
			return;
		undoRedo->noPriorUndo();
	}

	void clearHistory()
	{
		if (undoRedo == 0)
			return;
		undoRedo->clearHistory();
	}
}

