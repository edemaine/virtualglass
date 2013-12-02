
#ifndef UNDOREDO_H
#define UNDOREDO_H

#include <stack>
#include <vector>

#include "primitives.h"
#include "canetemplate.h"
#include "casing.h"
#include "subcanetemplate.h"

class QVBoxLayout;

class MainWindow;
class GlassColor;
class Cane;
class Piece;

using std::stack;
using std::vector;

class UndoRedo
{
	public:
		enum EventType
		{
			ADD,
			DELETE,
			MODIFY,
			MOVE,
			NO_UNDO
		};

		enum ObjectType
		{
			GLASSCOLOR,
			CANE,
			PIECE
		};

		class Event
		{
			public:
				enum EventType type;	
				enum ObjectType objectType;	

				unsigned int index;
				int direction;

				GlassColor* glassColor;
				GlassColor* preGlassColorState;
				GlassColor* postGlassColorState;

				Cane* cane;
				Cane* preCaneState;
				Cane* postCaneState;

				Piece* piece;
				Piece* prePieceState;
				Piece* postPieceState;
		};

		UndoRedo(MainWindow* mw);
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

		void undo();
		void redo();
	
	private:
		stack<Event> undoStack;
		stack<Event> redoStack;

		MainWindow* mainWindow;	
		
		static Event nulledEvent();
		void movedObject(enum ObjectType objectType, unsigned int index, int direction);
		void addedOrDeletedObject(enum ObjectType objectType, bool added, void* ptr,  unsigned int index);
		Event mostRecentAddOrModifyEvent(enum ObjectType objectType, void* ptr);
		void clearRedoStack();
		void clearUndoStack();
		void undoGlassColorEvent(Event& event);
		void undoCaneEvent(Event& event);
		void undoPieceEvent(Event& event);
		void redoGlassColorEvent(Event& event);
		void redoCaneEvent(Event& event);
		void redoPieceEvent(Event& event);
		bool canUndo();
		bool canRedo();
};

#endif

