
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

void UndoRedo :: addedGlassColor(GlassColor* gc, unsigned int index)
{
	struct Event event;
	event.type = ADD_EVENT;
	event.objectType = GLASSCOLOR;	
	event.index = index;
	event.glassColor = gc;
	event.glassColorState2.color = gc->_color;
	event.glassColorState2.shortName = gc->_shortName;	
	event.glassColorState2.longName = gc->_longName;	

	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}

void UndoRedo :: addedCane(Cane* c, unsigned int index)
{
	struct Event event;
	event.type = ADD_EVENT;
	event.objectType = CANE;	
	event.index = index;
	event.cane = c;
	event.caneState2.type = c->_type;
	event.caneState2.casings = c->_casings;
	event.caneState2.count = c->_count;
	event.caneState2.twist = c->_twist;
	event.caneState2.subs = c->_subs;

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

void UndoRedo :: deletedCane(Cane* c, unsigned int index)
{
	struct Event event;
	event.type = DELETE_EVENT;
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
	event.glassColorState2.color = gc->_color;
	event.glassColorState2.shortName = gc->_shortName;
	event.glassColorState2.longName = gc->_longName;

	undoStack.push(event);
	clearRedoStack();
	mainWindow->updateUndoRedoEnabled(true, false);
}

void UndoRedo :: modifiedCane(Cane* c)
{
	struct Event event;
	event.type = MODIFY_EVENT;
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
		if (prevEvent.type != MODIFY_EVENT && prevEvent.type != ADD_EVENT)
		{
			tmpStack.push(undoStack.top());
			undoStack.pop();
			continue;
		}
		
		// At this point, guaranteed to be an ADD or MODIFY CANE event on our object
		// Use final state from that event as starting state for this one.
		event.caneState1.type = prevEvent.caneState2.type; 
		event.caneState1.casings = prevEvent.caneState2.casings;
		event.caneState1.count = prevEvent.caneState2.count; 
		event.caneState1.twist = prevEvent.caneState2.twist; 
		event.caneState1.subs = prevEvent.caneState2.subs; 
		break;
	}
	while (!tmpStack.empty()) // Reset undoStack to the way it was
	{
		undoStack.push(tmpStack.top());
		tmpStack.pop();
	}
	// end: set event.caneState1 value
	event.caneState2.type = c->_type; 
	event.caneState2.casings = c->_casings;
	event.caneState2.count = c->_count; 
	event.caneState2.twist = c->_twist; 
	event.caneState2.subs = c->_subs; 

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

void UndoRedo :: movedCane(unsigned int index, int direction)
{
	struct Event event;
	event.type = MOVE_EVENT;
	event.objectType = CANE;
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
		if (undoStack.top().type != DELETE_EVENT)
		{
			undoStack.pop();
			continue;
		}
		
		switch (undoStack.top().objectType)
		{
			case GLASSCOLOR:
				delete undoStack.top().glassColor;
				break;
			case CANE:
				delete undoStack.top().cane;
				break;
		}
		undoStack.pop();
	}
} 

void UndoRedo :: clearRedoStack()
{
	// pop everything, actually delete ADD_EVENT objects since
	// they are objects that aren't currently in the library and never will come back
	while (!redoStack.empty())
	{
		if (redoStack.top().type != ADD_EVENT)
		{
			redoStack.pop();
			continue;
		}

		switch (redoStack.top().objectType)
		{
			case GLASSCOLOR:
				delete redoStack.top().glassColor;
				break;
			case CANE:
				delete redoStack.top().cane;
				break;
		}
		redoStack.pop();
	}
}

void UndoRedo :: undoGlassColorEvent(struct Event& event)
{
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
			event.glassColor->_color = event.glassColorState1.color;
			event.glassColor->_shortName = event.glassColorState1.shortName;
			event.glassColor->_longName = event.glassColorState1.longName;
			mainWindow->updateLibrary(event.glassColor);
			mainWindow->updateCurrentEditor();
			break;
		case MOVE_EVENT:
		{
			QLayoutItem* w = mainWindow->glassColorLibraryLayout->takeAt(event.index);
			mainWindow->glassColorLibraryLayout->insertWidget(event.index + event.direction, w->widget());
			break;
		}
	}
}

void UndoRedo :: undoCaneEvent(struct Event& event)
{
	switch (event.type)
	{
		case ADD_EVENT:
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
		case DELETE_EVENT:
                        mainWindow->caneLibraryLayout->insertWidget(event.index, new CaneLibraryWidget(event.cane, mainWindow));
			break;
		case MODIFY_EVENT:
			event.cane->_type = event.caneState1.type;
			event.cane->_casings = event.caneState1.casings;
			event.cane->_count = event.caneState1.count;
			event.cane->_twist = event.caneState1.twist;
			event.cane->_subs = event.caneState1.subs;
			mainWindow->updateLibrary(event.cane);
			mainWindow->updateCurrentEditor();
			break;
		case MOVE_EVENT:
		{
			QLayoutItem* w = mainWindow->caneLibraryLayout->takeAt(event.index);
			mainWindow->caneLibraryLayout->insertWidget(event.index + event.direction, w->widget());
			break;
		}
	}

}

void UndoRedo :: undo()
{
	if (undoStack.empty())
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
			event.glassColor->_color = event.glassColorState2.color;
			event.glassColor->_shortName = event.glassColorState2.shortName;
			event.glassColor->_longName = event.glassColorState2.longName;
			mainWindow->updateLibrary(event.glassColor);
			mainWindow->updateCurrentEditor();
			break;
		case MOVE_EVENT:
		{
			QLayoutItem* w = mainWindow->glassColorLibraryLayout->takeAt(event.index);
			mainWindow->glassColorLibraryLayout->insertWidget(event.index + event.direction, w->widget());
			break;
		}
	}
}

void UndoRedo :: redoCaneEvent(struct Event& event)
{
	switch (event.type)
	{
		case ADD_EVENT:
                        mainWindow->caneLibraryLayout->insertWidget(event.index, new CaneLibraryWidget(event.cane, mainWindow));
			break;
		case DELETE_EVENT:
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
		case MODIFY_EVENT:
			event.cane->_type = event.caneState2.type;
			event.cane->_casings = event.caneState2.casings;
			event.cane->_count = event.caneState2.count;
			event.cane->_twist = event.caneState2.twist;
			event.cane->_subs = event.caneState2.subs;
			mainWindow->updateLibrary(event.cane);
			mainWindow->updateCurrentEditor();
			break;
		case MOVE_EVENT:
		{
			QLayoutItem* w = mainWindow->caneLibraryLayout->takeAt(event.index);
			mainWindow->caneLibraryLayout->insertWidget(event.index + event.direction, w->widget());
			break;
		}
	}

}

void UndoRedo :: redo()
{
	if (redoStack.empty())
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
