
#include <QVBoxLayout>
#include <QLayoutItem>
#include <QWidget>
#include <QApplication>

#include "mainwindow.h"
#include "glasscolor.h"
#include "cane.h"
#include "piece.h"
#include "glasscolorlibrarywidget.h"
#include "canelibrarywidget.h"
#include "piecelibrarywidget.h"
#include "coloreditorwidget.h"
#include "caneeditorwidget.h"
#include "pieceeditorwidget.h"
#include "undoredo.h"
	
UndoRedo :: UndoRedo(MainWindow* _mainWindow)
{
	this->mainWindow = _mainWindow;
}

void UndoRedo :: clearHistory()
{
	clearUndoStack();
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(false, false);
}

// Just sets stuff to null so removing an event from the queue has
// data properly freed.
UndoRedo::Event UndoRedo :: nulledEvent()
{
	UndoRedo::Event event;

	event.glassColor = NULL;
	event.preGlassColorState = NULL;
	event.postGlassColorState = NULL;
	event.cane = NULL;
	event.preCaneState = NULL;
	event.postCaneState = NULL;
	event.piece = NULL;
	event.prePieceState = NULL;
	event.postPieceState = NULL;

	return event;
}


void UndoRedo :: addedOrDeletedObject(enum ObjectType objectType, bool added, void* ptr,  unsigned int index)
{
	UndoRedo::Event event = nulledEvent();

    event.type = added ? ADD : DELETE;
	event.objectType = objectType;
	event.index = index;
	switch (objectType)
	{
		case GLASSCOLOR:
		{
			GlassColor* glassColor = static_cast<GlassColor*>(ptr);
			event.glassColor = glassColor;
			event.postGlassColorState = added ? glassColor->copy() : NULL;
			break;
		}
		case CANE:	
		{
			Cane* cane = static_cast<Cane*>(ptr);	
			event.cane = cane;
			event.postCaneState = added ? cane->copy() : NULL;
			break;
		}
		case PIECE:
		{
			Piece* piece = static_cast<Piece*>(ptr);
			event.piece = piece;
			event.postPieceState = added ? piece->copy() : NULL;
			break;
		}
	}

	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}


void UndoRedo :: addedGlassColor(GlassColor* glassColor, unsigned int index)
{
	addedOrDeletedObject(GLASSCOLOR, true, glassColor, index);
}

void UndoRedo :: addedCane(Cane* cane, unsigned int index)
{
	addedOrDeletedObject(CANE, true, cane, index);
}

void UndoRedo :: addedPiece(Piece* piece, unsigned int index)
{
	addedOrDeletedObject(PIECE, true, piece, index);
}

void UndoRedo :: deletedGlassColor(GlassColor* glassColor, unsigned int index)
{
	addedOrDeletedObject(GLASSCOLOR, false, glassColor, index);
}

void UndoRedo :: deletedCane(Cane* cane, unsigned int index)
{
	addedOrDeletedObject(CANE, false, cane, index);
}

void UndoRedo :: deletedPiece(Piece* piece, unsigned int index)
{
	addedOrDeletedObject(PIECE, false, piece, index);
}

UndoRedo::Event UndoRedo :: mostRecentAddOrModifyEvent(enum ObjectType objectType, void* ptr)
{
	UndoRedo::Event event;

	stack<UndoRedo::Event> tmpStack;
	bool found = false;
	while (!found)
	{
		UndoRedo::Event prevEvent = undoStack.top();
			
		if (prevEvent.type != MODIFY && prevEvent.type != ADD)
		{
			tmpStack.push(undoStack.top());
			undoStack.pop();
			continue;
		}
	
		switch (objectType)
		{
			case GLASSCOLOR:
			{
				GlassColor* glassColor = static_cast<GlassColor*>(ptr);
				if (prevEvent.glassColor == glassColor)
					found = true;	
				break;
			}	
			case CANE:
			{
				Cane* cane = static_cast<Cane*>(ptr);
				if (prevEvent.cane == cane)
					found = true;
				break;
			}	
			case PIECE:
			{
				Piece* piece = static_cast<Piece*>(ptr);
				if (prevEvent.piece == piece)
					found = true;
				break;
			}
		}

		if (found)
			event = prevEvent;
		else
		{
			tmpStack.push(undoStack.top());
			undoStack.pop();
		}
	}

	// restore the undoStack
	while (!tmpStack.empty()) 
	{
		undoStack.push(tmpStack.top());
		tmpStack.pop();
	}

	return event;
}

void UndoRedo :: modifiedGlassColor(GlassColor* glassColor)
{
	UndoRedo::Event event = nulledEvent();

	event.type = MODIFY;
	event.objectType = GLASSCOLOR;	
	event.glassColor = glassColor;	

	UndoRedo::Event prevEvent = mostRecentAddOrModifyEvent(GLASSCOLOR, glassColor);
	event.preGlassColorState = prevEvent.postGlassColorState->copy();
	event.postGlassColorState = glassColor->copy();

	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}

void UndoRedo :: modifiedCane(Cane* cane)
{
	UndoRedo::Event event = nulledEvent();

	event.type = MODIFY;
	event.objectType = CANE;	
	event.cane = cane;

	UndoRedo::Event prevEvent = mostRecentAddOrModifyEvent(CANE, cane);
	event.preCaneState = prevEvent.postCaneState->copy();
	event.postCaneState = cane->copy();

	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}

void UndoRedo :: modifiedPiece(Piece* piece)
{
	UndoRedo::Event event = nulledEvent();

	event.type = MODIFY;
	event.objectType = PIECE;	
	event.piece = piece;

	UndoRedo::Event prevEvent = mostRecentAddOrModifyEvent(PIECE, piece);
	event.prePieceState = prevEvent.postPieceState->copy();
	event.postPieceState = piece->copy();

	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}

void UndoRedo :: movedObject(enum ObjectType objectType, unsigned int index, int direction)
{
	UndoRedo::Event event = nulledEvent();

	event.type = MOVE;
	event.objectType = objectType;
	event.index = index;
	event.direction = direction;
	
	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}

void UndoRedo :: movedGlassColor(unsigned int index, int direction)
{
	movedObject(GLASSCOLOR, index, direction);
}

void UndoRedo :: movedCane(unsigned int index, int direction)
{
	movedObject(CANE, index, direction);
}

void UndoRedo :: movedPiece(unsigned int index, int direction)
{
	movedObject(PIECE, index, direction);
}

void UndoRedo :: clearUndoStack()
{
	while (!undoStack.empty())
	{
		UndoRedo::Event event = undoStack.top();

		if (event.preGlassColorState != NULL)
			delete event.preGlassColorState;
		if (event.postGlassColorState != NULL)
			delete event.postGlassColorState;
		if (event.preCaneState != NULL)
			delete event.preCaneState;
		if (event.postCaneState != NULL)
			delete event.postCaneState;
		if (event.prePieceState != NULL)
			delete event.prePieceState;
		if (event.postPieceState != NULL)
			delete event.postPieceState;

		// actually delete DELETE objects since they are objects that 
		// aren't currently in the library and never will come back
		if (event.type == DELETE)
		{
			if (event.glassColor != NULL)
				delete event.glassColor;
			if (event.cane != NULL)
				delete event.cane;
			if (event.piece != NULL)
				delete event.piece;
		}	

		undoStack.pop();
	}
} 

void UndoRedo :: clearRedoStack()
{
	while (!redoStack.empty())
	{
		UndoRedo::Event event = redoStack.top();

		if (event.preGlassColorState != NULL)
			delete event.preGlassColorState;
		if (event.postGlassColorState != NULL)
			delete event.postGlassColorState;
		if (event.preCaneState != NULL)
			delete event.preCaneState;
		if (event.postCaneState != NULL)
			delete event.postCaneState;
		if (event.prePieceState != NULL)
			delete event.prePieceState;
		if (event.postPieceState != NULL)
			delete event.postPieceState;

		// actually delete ADD objects since they are objects that 
		// aren't currently in the library and never will come back
		if (event.type == ADD)
		{
			if (event.glassColor != NULL)
				delete event.glassColor;
			if (event.cane != NULL)
				delete event.cane;
			if (event.piece != NULL)
				delete event.piece;
		}	
		
		redoStack.pop();
	}
}

void UndoRedo :: undoGlassColorEvent(UndoRedo::Event& event)
{
	switch (event.type)
	{
		case ADD:
		{
			if (mainWindow->glassColorLibraryLayout->count() > 1)
			{
				int r = (event.index == 0) ? 1 : event.index-1;
				mainWindow->glassColorEditorWidget->setGlassColor(dynamic_cast<GlassColorLibraryWidget*>(
					dynamic_cast<QWidgetItem*>(mainWindow->glassColorLibraryLayout->itemAt(r))->widget())->glassColor);
				mainWindow->glassObjectModified();
			}
			else
				mainWindow->setViewMode(MainWindow::EMPTY_VIEW_MODE);

			QLayoutItem* cur = mainWindow->glassColorLibraryLayout->takeAt(event.index);
			cur->widget()->moveToThread(QApplication::instance()->thread());
			cur->widget()->deleteLater();
			break;
		}
		case DELETE:
			mainWindow->glassColorLibraryLayout->insertWidget(
				event.index, new GlassColorLibraryWidget(event.glassColor, mainWindow));
			mainWindow->glassObjectModified();
			break;
		case MODIFY:
			event.glassColor->set(event.preGlassColorState);
			break;
		case MOVE:
		{
			QLayoutItem* w = mainWindow->glassColorLibraryLayout->takeAt(event.index);
			mainWindow->glassColorLibraryLayout->insertWidget(event.index + event.direction, w->widget());
			break;
		}
		case NO_UNDO:
			break;
	}
}

void UndoRedo :: undoCaneEvent(UndoRedo::Event& event)
{
	switch (event.type)
	{
		case ADD:
		{
			if (mainWindow->caneLibraryLayout->count() > 1)
			{
				int r = (event.index == 0) ? 1 : event.index-1;
				mainWindow->caneEditorWidget->setCane(dynamic_cast<CaneLibraryWidget*>(
					dynamic_cast<QWidgetItem*>(mainWindow->caneLibraryLayout->itemAt(r))->widget())->cane);
				mainWindow->glassObjectModified();
			}
			else
				mainWindow->setViewMode(MainWindow::EMPTY_VIEW_MODE);

			QLayoutItem* cur = mainWindow->caneLibraryLayout->takeAt(event.index);
			cur->widget()->moveToThread(QApplication::instance()->thread());
			cur->widget()->deleteLater();
			break;
		}
		case DELETE:
			mainWindow->caneLibraryLayout->insertWidget(event.index, new CaneLibraryWidget(event.cane, mainWindow));
			mainWindow->glassObjectModified();
			break;
		case MODIFY:
			event.cane->set(event.preCaneState);
			break;
		case MOVE:
		{
			QLayoutItem* w = mainWindow->caneLibraryLayout->takeAt(event.index);
			mainWindow->caneLibraryLayout->insertWidget(event.index + event.direction, w->widget());
			break;
		}
		case NO_UNDO:
			break;
	}
}

void UndoRedo :: undoPieceEvent(UndoRedo::Event& event)
{
	switch (event.type)
	{
		case ADD:
		{
			if (mainWindow->pieceLibraryLayout->count() > 1)
			{
				int r = (event.index == 0) ? 1 : event.index-1;
				mainWindow->pieceEditorWidget->setPiece(dynamic_cast<PieceLibraryWidget*>(
					dynamic_cast<QWidgetItem*>(mainWindow->pieceLibraryLayout->itemAt(r))->widget())->piece);
				mainWindow->glassObjectModified();
			}
			else
				mainWindow->setViewMode(MainWindow::EMPTY_VIEW_MODE);

			QLayoutItem* cur = mainWindow->pieceLibraryLayout->takeAt(event.index);
			cur->widget()->moveToThread(QApplication::instance()->thread());
			cur->widget()->deleteLater();
			break;
		}
		case DELETE:
			mainWindow->pieceLibraryLayout->insertWidget(event.index, new PieceLibraryWidget(event.piece, mainWindow));
			mainWindow->glassObjectModified();
			break;
		case MODIFY:
			event.piece->set(event.prePieceState);
			break;
		case MOVE:
		{
			QLayoutItem* w = mainWindow->pieceLibraryLayout->takeAt(event.index);
			mainWindow->pieceLibraryLayout->insertWidget(event.index + event.direction, w->widget());
			break;
		}
		case NO_UNDO:
			break;
	}
}

// This method forms a temporal "cut", where undo-ing backwards past the
// point that this method is called is forbidden.
// The reason for keeping the remainder of the undo stack around is that
// it contains ADD and MODIFY events needed by new MODIFY events for 
// prior state of an object.
//
// noPriorUndo() should be used after a sequence of non-user performed actions
// that change the library or objects, e.g. adding the initial default objects
// after pressing "New File" or adding objects from a file to the existing library. 
//
// The NO_UNDO events never make it into the redoStack, as they start in the undoStack
// and cannot be processed to be pushed into the redoStack.
void UndoRedo :: noPriorUndo()
{
	UndoRedo::Event event = nulledEvent();

	event.type = NO_UNDO;

	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(false, false);
}

bool UndoRedo :: canUndo()
{
	return (!undoStack.empty() && undoStack.top().type != NO_UNDO);
}

void UndoRedo :: undo()
{
	if (!canUndo())
		return;

	UndoRedo::Event event = undoStack.top();
	switch (undoStack.top().objectType)
	{
		case GLASSCOLOR:
			undoGlassColorEvent(event);
			break;
		case CANE:
			undoCaneEvent(event);
			break;
		case PIECE:
			undoPieceEvent(event);
			break;
	}
	redoStack.push(undoStack.top());
	undoStack.pop();
	mainWindow->updateUndoRedoEnabled(!undoStack.empty(), !redoStack.empty());
	mainWindow->setDirtyBit(true);
}


void UndoRedo :: redoGlassColorEvent(UndoRedo::Event& event)
{
	switch (event.type)
	{
		case ADD:
			mainWindow->glassColorLibraryLayout->insertWidget(
				event.index, new GlassColorLibraryWidget(event.glassColor, mainWindow));
			mainWindow->glassObjectModified();
			break;
		case DELETE:
		{
			if (mainWindow->glassColorLibraryLayout->count() > 1)
			{
				int r = (event.index == 0) ? 1 : event.index-1;
				mainWindow->glassColorEditorWidget->setGlassColor(dynamic_cast<GlassColorLibraryWidget*>(
					dynamic_cast<QWidgetItem*>(mainWindow->glassColorLibraryLayout->itemAt(r))->widget())->glassColor);
				mainWindow->glassObjectModified();
			}
			else
				mainWindow->setViewMode(MainWindow::EMPTY_VIEW_MODE);

			QLayoutItem* cur = mainWindow->glassColorLibraryLayout->takeAt(event.index);
			cur->widget()->moveToThread(QApplication::instance()->thread());
			cur->widget()->deleteLater();
			break;
		}
		case MODIFY:
			event.glassColor->set(event.postGlassColorState);
			break;
		case MOVE:
		{
			QLayoutItem* w = mainWindow->glassColorLibraryLayout->takeAt(event.index);
			mainWindow->glassColorLibraryLayout->insertWidget(event.index + event.direction, w->widget());
			break;
		}
		case NO_UNDO:
			break;
	}
}

void UndoRedo :: redoCaneEvent(UndoRedo::Event& event)
{
	switch (event.type)
	{
		case ADD:
			mainWindow->caneLibraryLayout->insertWidget(event.index, new CaneLibraryWidget(event.cane, mainWindow));
			mainWindow->glassObjectModified();
			break;
		case DELETE:
		{
			if (mainWindow->caneLibraryLayout->count() > 1)
			{
				int r = (event.index == 0) ? 1 : event.index-1;
				mainWindow->caneEditorWidget->setCane(dynamic_cast<CaneLibraryWidget*>(
					dynamic_cast<QWidgetItem*>(mainWindow->caneLibraryLayout->itemAt(r))->widget())->cane);
				mainWindow->glassObjectModified();
			}
			else
				mainWindow->setViewMode(MainWindow::EMPTY_VIEW_MODE);

			QLayoutItem* cur = mainWindow->caneLibraryLayout->takeAt(event.index);
			cur->widget()->moveToThread(QApplication::instance()->thread());
			cur->widget()->deleteLater();
			break;
		}
		case MODIFY:
			event.cane->set(event.postCaneState);
			break;
		case MOVE:
		{
			QLayoutItem* w = mainWindow->caneLibraryLayout->takeAt(event.index);
			mainWindow->caneLibraryLayout->insertWidget(event.index + event.direction, w->widget());
			break;
		}
		case NO_UNDO:
			break;
	}

}

void UndoRedo :: redoPieceEvent(UndoRedo::Event& event)
{
	switch (event.type)
	{
		case ADD:
			mainWindow->pieceLibraryLayout->insertWidget(event.index, new PieceLibraryWidget(event.piece, mainWindow));
			mainWindow->glassObjectModified();
			break;
		case DELETE:
		{
			if (mainWindow->pieceLibraryLayout->count() > 1)
			{
				int r = (event.index == 0) ? 1 : event.index-1;
				mainWindow->pieceEditorWidget->setPiece(dynamic_cast<PieceLibraryWidget*>(
					dynamic_cast<QWidgetItem*>(mainWindow->pieceLibraryLayout->itemAt(r))->widget())->piece);
				mainWindow->glassObjectModified();
			}
			else
				mainWindow->setViewMode(MainWindow::EMPTY_VIEW_MODE);

			QLayoutItem* cur = mainWindow->pieceLibraryLayout->takeAt(event.index);
			cur->widget()->moveToThread(QApplication::instance()->thread());
			cur->widget()->deleteLater();
			break;
		}
		case MODIFY:
			event.piece->set(event.postPieceState);
			break;
		case MOVE:
		{
			QLayoutItem* w = mainWindow->pieceLibraryLayout->takeAt(event.index);
			mainWindow->pieceLibraryLayout->insertWidget(event.index + event.direction, w->widget());
			break;
		}
		case NO_UNDO:
			break;
	}

}

bool UndoRedo :: canRedo()
{
	return !redoStack.empty();
}

void UndoRedo :: redo()
{
	if (!canRedo())
		return;

	UndoRedo::Event event = redoStack.top();
	switch (event.objectType)
	{
		case GLASSCOLOR:
			redoGlassColorEvent(event);
			break;
		case CANE:
			redoCaneEvent(event);
			break;
		case PIECE:
			redoPieceEvent(event);
			break;
	}
	undoStack.push(redoStack.top());
	redoStack.pop();
	mainWindow->updateUndoRedoEnabled(!undoStack.empty(), !redoStack.empty());
	mainWindow->setDirtyBit(true);
}
