
#ifndef GLOBALUNDOREDO_H
#define GLOBALUNDOREDO_H 

class UndoRedo;
class MainWindow;
class GlassColor;
class Cane;
class Piece;

namespace GlobalUndoRedo
{
	void setMainWindow(MainWindow* mw);

	void undo();
	void redo();
	
	void addedGlassColor(GlassColor* gc, unsigned int index);
	void deletedGlassColor(GlassColor* gc, unsigned int index);
	void modifiedGlassColor(GlassColor* gc);
	void movedGlassColor(unsigned int index, int direction);
	void addedCane(Cane* c, unsigned int index);
	void deletedCane(Cane* c, unsigned int index);
	void modifiedCane(Cane* c);
	void movedCane(unsigned int index, int direction);
	void addedPiece(Piece* p, unsigned int index);
	void deletedPiece(Piece* p, unsigned int index);
	void modifiedPiece(Piece* p);
	void movedPiece(unsigned int index, int direction);

	void noPriorUndo();
	void clearHistory();
}

#endif

