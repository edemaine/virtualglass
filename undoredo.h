
#ifndef UNDOREDO_H
#define UNDOREDO_H

#include <stack>
#include <string>

#include <QObject>

#include "primitives.h"

class QVBoxLayout;

class MainWindow;
class GlassColor;
class Cane;
class Piece;

using std::string;
using std::stack;

class UndoRedo : public QObject
{
	Q_OBJECT

	public:
		enum EventType
		{
			ADD_EVENT,
			DELETE_EVENT,
			MODIFY_EVENT,
			MOVE_EVENT
		};

		enum ObjectType
		{
			GLASSCOLOR,
			CANE,
			PIECE
		};

		struct GlassColorState
		{
			Color color;
			string shortName;
			string longName;
		};

		struct Event
		{
			enum EventType type;	
			enum ObjectType objectType;	

			unsigned int index;
			int direction;

			GlassColor* glassColor;
			struct GlassColorState glassColorState1;
			struct GlassColorState glassColorState2;
		};

		UndoRedo(MainWindow* mw);

		void addedGlassColor(GlassColor* gc, unsigned int index);
		void deletedGlassColor(GlassColor* gc, unsigned int index);
		void modifiedGlassColor(GlassColor* gc);
		void movedGlassColor(unsigned int index, int direction);

		/*
		void newCane(Cane* c);
		void deleteCane(Cane* c);
		void modifiedCane(Cane* c);
		void newPiece(Piece* p);
		void deletePiece(Piece* p);
		void modifiedPiece(Piece* p);
		*/

		bool canUndo();
		bool canRedo();

	public slots:
		void undo();
		void redo();
	
	private:
		stack<struct Event> undoStack;
		stack<struct Event> redoStack;

		MainWindow* mainWindow;	

		void clearRedoStack();
};

#endif

