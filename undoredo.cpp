
#include <QVBoxLayout>
#include <QLayoutItem>
#include <QWidget>
#include <QApplication>


#include "undoredo.h"
#include "glasscolor.h"
#include "cane.h"
#include "piece.h"
#include "glasscolorlibrarywidget.h"
#include "mainwindow.h"
#include "coloreditorwidget.h"
	
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

void UndoRedo :: addedGlassColor(GlassColor* gc, unsigned int index)
{
	struct Event event;
	event.type = ADD_EVENT;
	event.objectType = GLASSCOLOR;	
	event.index = index;
	event.glassColor = gc;
	event.glassColorState2.color = gc->color();
	event.glassColorState2.shortName = gc->shortName();	
	event.glassColorState2.longName = gc->longName();	

	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}

void UndoRedo :: deletedGlassColor(GlassColor* gc, unsigned int index)
{
	struct Event event;
	event.type = DELETE_EVENT;
	event.objectType = GLASSCOLOR;	
	event.index = index;
	event.glassColor = gc;

	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}

void UndoRedo :: modifiedGlassColor(GlassColor* gc)
{
	struct Event event;
	event.type = MODIFY_EVENT;
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
		if (prevEvent.type != MODIFY_EVENT && prevEvent.type != ADD_EVENT)
		{
			tmpStack.push(undoStack.top());
			undoStack.pop();
			continue;
		}
		
		// At this point, guaranteed to be an ADD or MODIFY GLASSCOLOR event on our object
		// Use final state from that event as starting state for this one.
		event.glassColorState1.color = prevEvent.glassColorState2.color; 
		event.glassColorState1.shortName = prevEvent.glassColorState2.shortName; 
		event.glassColorState1.longName = prevEvent.glassColorState2.longName; 
		break;
	}
	while (!tmpStack.empty()) // Reset undoStack to the way it was
	{
		undoStack.push(tmpStack.top());
		tmpStack.pop();
	}
	// end: set event.glassColorState1 value
	event.glassColorState2.color = gc->color();
	event.glassColorState2.shortName = gc->shortName();
	event.glassColorState2.longName = gc->longName();

	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}

void UndoRedo :: movedGlassColor(unsigned int index, int direction)
{
	struct Event event;
	event.type = MOVE_EVENT;
	event.objectType = GLASSCOLOR;
	event.index = index;
	event.direction = direction;
	
	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}

void UndoRedo :: clearUndoStack()
{
	// pop everything, actually delete DELETE_EVENT objects since
	// they are objects that aren't currently in the library and never will come back
	while (!undoStack.empty())
	{
		if (undoStack.top().type == DELETE_EVENT)
			delete undoStack.top().glassColor;
		undoStack.pop();
	}
} 

void UndoRedo :: clearRedoStack()
{
	// pop everything, actually delete ADD_EVENT objects since
	// they are objects that aren't currently in the library and never will come back
	while (!redoStack.empty())
	{
		if (redoStack.top().type == ADD_EVENT)
			delete redoStack.top().glassColor;
		redoStack.pop();
	}
}

void UndoRedo :: undo()
{
	if (undoStack.empty())
		return;

	struct Event event = undoStack.top();
	switch (event.type)
	{
		case ADD_EVENT:
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
		case DELETE_EVENT:
                        mainWindow->glassColorLibraryLayout->insertWidget(
				event.index, new GlassColorLibraryWidget(event.glassColor, mainWindow));
			break;
		case MODIFY_EVENT:
			event.glassColor->setColor(event.glassColorState1.color);
			event.glassColor->setShortName(event.glassColorState1.shortName);
			event.glassColor->setLongName(event.glassColorState1.longName);
			mainWindow->updateLibrary(event.glassColor);
			break;
		case MOVE_EVENT:
		{
			QLayoutItem* w = mainWindow->glassColorLibraryLayout->takeAt(event.index);
			mainWindow->glassColorLibraryLayout->insertWidget(event.index + event.direction, w->widget());
			break;
		}
	}
	redoStack.push(undoStack.top());
	undoStack.pop();
	mainWindow->updateUndoRedoEnabled(!undoStack.empty(), !redoStack.empty());
	mainWindow->setDirtyBit(true);
}

void UndoRedo :: redo()
{
	if (redoStack.empty())
		return;
	struct Event event = redoStack.top();

	switch (event.type)
	{
		case ADD_EVENT:
                        mainWindow->glassColorLibraryLayout->insertWidget(
				event.index, new GlassColorLibraryWidget(event.glassColor, mainWindow));
			break;
		case DELETE_EVENT:
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
		case MODIFY_EVENT:
			event.glassColor->setColor(event.glassColorState2.color);
			event.glassColor->setShortName(event.glassColorState2.shortName);
			event.glassColor->setLongName(event.glassColorState2.longName);
			mainWindow->updateLibrary(event.glassColor);
			break;
		case MOVE_EVENT:
		{
			QLayoutItem* w = mainWindow->glassColorLibraryLayout->takeAt(event.index);
			mainWindow->glassColorLibraryLayout->insertWidget(event.index + event.direction, w->widget());
			break;
		}
	}
	undoStack.push(redoStack.top());
	redoStack.pop();
	mainWindow->updateUndoRedoEnabled(!undoStack.empty(), !redoStack.empty());
	mainWindow->setDirtyBit(true);
}



