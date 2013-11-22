
#include <QVBoxLayout>
#include <QLayoutItem>
#include <QWidget>
#include <QApplication>

#include "undoredo.h"
#include "mainwindow.h"
#include "glasscolor.h"
#include "cane.h"
#include "piece.h"
#include "glasscolorlibrarywidget.h"
#include "canelibrarywidget.h"
#include "coloreditorwidget.h"
#include "caneeditorwidget.h"
	
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
void UndoRedo :: initEvent(struct Event& event)
{
	event.glassColor = NULL;
	event.preGlassColorState = NULL;
	event.postGlassColorState = NULL;
	event.cane = NULL;
	event.preCaneState = NULL;
	event.postCaneState = NULL;
}

void UndoRedo :: addedGlassColor(GlassColor* gc, unsigned int index)
{
	struct Event event;
	event.type = ADD;
	event.objectType = GLASSCOLOR;	
	event.index = index;
	event.glassColor = gc;
	event.postGlassColorState = gc->copy();

	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}

void UndoRedo :: addedCane(Cane* c, unsigned int index)
{
	struct Event event;
	event.type = ADD;
	event.objectType = CANE;	
	event.index = index;
	event.cane = c;
	event.postCaneState = c->copy();

	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}

void UndoRedo :: deletedGlassColor(GlassColor* gc, unsigned int index)
{
	struct Event event;
	event.type = DELETE;
	event.objectType = GLASSCOLOR;	
	event.index = index;
	event.glassColor = gc;

	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}

void UndoRedo :: deletedCane(Cane* c, unsigned int index)
{
	struct Event event;
	event.type = DELETE;
	event.objectType = CANE;	
	event.index = index;
	event.cane = c;

	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}

void UndoRedo :: modifiedGlassColor(GlassColor* gc)
{
	struct Event event;
	event.type = MODIFY;
	event.objectType = GLASSCOLOR;	
	event.glassColor = gc;	
	stack<struct Event> tmpStack;
	// start: set event.glassColorState1 value
	while (true)
	{
		struct Event prevEvent = undoStack.top();
		if (prevEvent.objectType != GLASSCOLOR || prevEvent.glassColor != gc)
		{
			tmpStack.push(undoStack.top());
			undoStack.pop();
			continue;
		}
		if (prevEvent.type != MODIFY && prevEvent.type != ADD)
		{
			tmpStack.push(undoStack.top());
			undoStack.pop();
			continue;
		}
		
		// At this point, guaranteed to be an ADD or MODIFY GLASSCOLOR event on our object
		// Use final state from that event as starting state for this one.
		event.preGlassColorState = prevEvent.postGlassColorState->copy(); 
		break;
	}
	while (!tmpStack.empty()) // Reset undoStack to the way it was
	{
		undoStack.push(tmpStack.top());
		tmpStack.pop();
	}
	// end: set event.glassColorState1 value
	event.postGlassColorState = gc->copy();

	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}

void UndoRedo :: modifiedCane(Cane* c)
{
	struct Event event;
	event.type = MODIFY;
	event.objectType = CANE;	
	event.cane = c;
	stack<struct Event> tmpStack;
	// start: set event.caneState1 value
	while (true)
	{
		struct Event prevEvent = undoStack.top();
		if (prevEvent.objectType != CANE || prevEvent.cane != c)
		{
			tmpStack.push(undoStack.top());
			undoStack.pop();
			continue;
		}
		if (prevEvent.type != MODIFY && prevEvent.type != ADD)
		{
			tmpStack.push(undoStack.top());
			undoStack.pop();
			continue;
		}
		
		// At this point, guaranteed to be an ADD or MODIFY CANE event on our object
		// Use final state from that event as starting state for this one.
		event.preCaneState = prevEvent.postCaneState->copy();
		break;
	}
	while (!tmpStack.empty()) // Reset undoStack to the way it was
	{
		undoStack.push(tmpStack.top());
		tmpStack.pop();
	}
	// end: set event.caneState1 value
	event.postCaneState = c->copy();

	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}

void UndoRedo :: movedGlassColor(unsigned int index, int direction)
{
	struct Event event;
	event.type = MOVE;
	event.objectType = GLASSCOLOR;
	event.index = index;
	event.direction = direction;
	
	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}

void UndoRedo :: movedCane(unsigned int index, int direction)
{
	struct Event event;
	event.type = MOVE;
	event.objectType = CANE;
	event.index = index;
	event.direction = direction;
	
	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}

void UndoRedo :: clearUndoStack()
{
	// pop everything, actually delete DELETE objects since
	// they are objects that aren't currently in the library and never will come back
	while (!undoStack.empty())
	{
		struct Event event = undoStack.top();
		switch (event.objectType)
		{
			case GLASSCOLOR:
			{
				switch (event.type)
				{
					case ADD:
						delete event.postGlassColorState;
						break;
					case DELETE: 
						delete event.glassColor;
						break;
					case MODIFY:
						delete event.preGlassColorState;
						delete event.postGlassColorState;
						break;
					case MOVE:
					case NO_UNDO: 
						break;
				}
				break;
			}
			case CANE:
			{
				switch (event.type)
				{
					case ADD:
						delete event.postCaneState;
						break;
					case DELETE:
						delete event.cane;
						break;
					case MODIFY:
						delete event.preCaneState;
						delete event.postCaneState;
						break;
					case MOVE:
					case NO_UNDO:
						break;
				}	
				break;
			}
		}
		undoStack.pop();
	}
} 

void UndoRedo :: clearRedoStack()
{
	// pop everything, actually delete ADD objects since
	// they are objects that aren't currently in the library and never will come back
	while (!redoStack.empty())
	{
		struct Event event = redoStack.top();
		switch (event.objectType)
		{
			case GLASSCOLOR:
			{
				switch (event.type)
				{
					case ADD:
						delete event.glassColor;
						break;
					case DELETE: 
						break;
					case MODIFY:
						delete event.preGlassColorState;
						delete event.postGlassColorState;
						break;
					case MOVE:
					case NO_UNDO: 
						break;
				}
				break;
			}
			case CANE:
			{
				switch (event.type)
				{
					case ADD:
						delete event.cane;
						break;
					case DELETE:
						break;
					case MODIFY:
						delete event.preCaneState;
						delete event.postCaneState;
						break;
					case MOVE:
					case NO_UNDO:
						break;
				}	
				break;
			}
		}
		redoStack.pop();
	}
}

void UndoRedo :: undoGlassColorEvent(struct Event& event)
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
			break;
		case MODIFY:
			event.glassColor->set(event.preGlassColorState);
			mainWindow->updateLibrary(event.glassColor);
			mainWindow->updateCurrentEditor();
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

void UndoRedo :: undoCaneEvent(struct Event& event)
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
			break;
		case MODIFY:
			event.cane->set(event.preCaneState);
			mainWindow->updateLibrary(event.cane);
			mainWindow->updateCurrentEditor();
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
	struct Event event;
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

	struct Event event = undoStack.top();
	switch (undoStack.top().objectType)
	{
		case GLASSCOLOR:
			undoGlassColorEvent(event);
			break;
		case CANE:
			undoCaneEvent(event);
			break;
	}
	redoStack.push(undoStack.top());
	undoStack.pop();
	mainWindow->updateUndoRedoEnabled(!undoStack.empty(), !redoStack.empty());
	mainWindow->setDirtyBit(true);
}


void UndoRedo :: redoGlassColorEvent(struct Event& event)
{
	switch (event.type)
	{
		case ADD:
                        mainWindow->glassColorLibraryLayout->insertWidget(
				event.index, new GlassColorLibraryWidget(event.glassColor, mainWindow));
			break;
		case DELETE:
		{
			if (mainWindow->glassColorLibraryLayout->count() > 1)
			{
				int r = (event.index == 0) ? 1 : event.index-1;
				mainWindow->glassColorEditorWidget->setGlassColor(dynamic_cast<GlassColorLibraryWidget*>(
					dynamic_cast<QWidgetItem*>(mainWindow->glassColorLibraryLayout->itemAt(r))->widget())->glassColor);
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
			mainWindow->updateLibrary(event.glassColor);
			mainWindow->updateCurrentEditor();
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

void UndoRedo :: redoCaneEvent(struct Event& event)
{
	switch (event.type)
	{
		case ADD:
                        mainWindow->caneLibraryLayout->insertWidget(event.index, new CaneLibraryWidget(event.cane, mainWindow));
			break;
		case DELETE:
		{
			if (mainWindow->caneLibraryLayout->count() > 1)
			{
				int r = (event.index == 0) ? 1 : event.index-1;
				mainWindow->caneEditorWidget->setCane(dynamic_cast<CaneLibraryWidget*>(
					dynamic_cast<QWidgetItem*>(mainWindow->caneLibraryLayout->itemAt(r))->widget())->cane);
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
			mainWindow->updateLibrary(event.cane);
			mainWindow->updateCurrentEditor();
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

bool UndoRedo :: canRedo()
{
	return !redoStack.empty();
}

void UndoRedo :: redo()
{
	if (!canRedo())
		return;

	struct Event event = redoStack.top();
	switch (event.objectType)
	{
		case GLASSCOLOR:
			redoGlassColorEvent(event);
			break;
		case CANE:
			redoCaneEvent(event);
			break;
	}
	undoStack.push(redoStack.top());
	redoStack.pop();
	mainWindow->updateUndoRedoEnabled(!undoStack.empty(), !redoStack.empty());
	mainWindow->setDirtyBit(true);
}
