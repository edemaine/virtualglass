
#include <QImageWriter>
#include <QImage>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QStackedWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QPushButton>
#include <QMouseEvent>
#include <QTextStream>
#include <QApplication>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QStatusBar>
#include <QToolButton>
#include <QScrollBar>
#include <QSizePolicy>
#include <QToolBar>
#include <QInputDialog>
#include <QLineEdit>
#include <QBuffer>

#include "constants.h"
#include "dependancy.h"
#include "email.h"
#include "niceviewwidget.h"
#include "piecelibrarywidget.h"
#include "canelibrarywidget.h"
#include "glasscolorlibrarywidget.h"
#include "cane.h"
#include "pulltemplate.h"
#include "pickup.h"
#include "piece.h"
#include "caneeditorwidget.h"
#include "coloreditorwidget.h"
#include "pieceeditorwidget.h"
#include "randomglass.h"
#include "glassmime.h"
#include "pulltemplate.h"
#include "mainwindow.h"
#include "globalglass.h"
#include "SVG.hpp"
#include "glassfileio.h"
#include "globalgraphicssetting.h"
#include "globaldepthpeelingsetting.h"

MainWindow :: MainWindow()
{
	centralWidget = new QWidget(this);
	this->setCentralWidget(centralWidget);

	// allocate ALL the memory
	centralLayout = new QHBoxLayout(centralWidget);
	setupLibrary();
	setupEditors();
	setupMenus();
	setupSaveFile();
	setupStatusBar();
	setupConnections();
	setWindowTitle(windowTitle());

	// do the finishing touches to put the GUI in fresh state
	setViewMode(EMPTY_VIEW_MODE);
	setDirtyBit(false);

	showMaximized();
}

void MainWindow :: setViewMode(enum ViewMode _mode)
{
	editorStack->setCurrentIndex(_mode);

	switch (_mode)
	{
		case EMPTY_VIEW_MODE:
			exportPLYFileAction->setEnabled(false);
			exportOBJFileAction->setEnabled(false);
			saveSelectedAsFileAction->setEnabled(false);
			shareFileButton->setEnabled(false);
			break;
		case COLORBAR_VIEW_MODE:
			exportPLYFileAction->setEnabled(false);
			exportOBJFileAction->setEnabled(false);
			saveSelectedAsFileAction->setEnabled(true);
			shareFileButton->setEnabled(false);
			break;
		case PULLPLAN_VIEW_MODE:
			caneEditorWidget->reset3DCamera();
			exportPLYFileAction->setEnabled(true);
			exportOBJFileAction->setEnabled(true);
			saveSelectedAsFileAction->setEnabled(true);
			shareFileButton->setEnabled(!email->sending());
			break;
		case PIECE_VIEW_MODE:
			pieceEditorWidget->reset3DCamera();
			exportPLYFileAction->setEnabled(true);
			exportOBJFileAction->setEnabled(true);
			saveSelectedAsFileAction->setEnabled(true);
			shareFileButton->setEnabled(!email->sending());
			break;
	}
	// zero out any status messages, as (currently, r957) they only pertain
	// to rendering of 3D views and so are view mode-specific.
	statusBar()->clearMessage();
}

QString MainWindow :: windowTitle()
{
	QString title = "VirtualGlass";
	QFile inFile(":/version.txt");
	if (inFile.open(QIODevice::ReadOnly)) {
		QTextStream in(&inFile);
		QString revision = in.readLine();
		QString date = in.readLine();
		title += " (r" + revision + ")";
	}
	if (dirtyBit)
		title += " *";
	title += " " + saveFilename;

	return title;
}

void MainWindow :: setupStatusBar()
{
	statusBar();
}

void MainWindow :: showStatusMessage(const QString& message, unsigned int timeout)
{
	statusBar()->showMessage(message, timeout * 1000);
}

void MainWindow :: keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
		case Qt::Key_Backspace:
		case Qt::Key_Delete:
			deleteCurrentEditingObject();
			event->accept();
			return;
	}
}

bool MainWindow :: eventFilter(QObject* obj, QEvent* event)
{
	if (obj == libraryScrollArea && event->type() == QEvent::KeyPress)
	{
		switch (dynamic_cast<QKeyEvent*>(event)->key())
		{
			case Qt::Key_Up:
				moveCurrentEditingObject(1);
				event->accept();
				return true;
			case Qt::Key_Down:	
				moveCurrentEditingObject(-1);
				event->accept();
				return true;
		}
	}
	return false;
}

void MainWindow :: moveCurrentEditingObject(int d)
{
	switch (editorStack->currentIndex())
	{
		case COLORBAR_VIEW_MODE:
			for (int i = 0; i < glassColorLibraryLayout->count(); ++i)
			{
				QLayoutItem* w = glassColorLibraryLayout->itemAt(i);
				GlassColor* gc = dynamic_cast<GlassColorLibraryWidget*>(w->widget())->glassColor;
				if (gc == glassColorEditorWidget->glassColor())
				{
					w = glassColorLibraryLayout->takeAt(i);
					glassColorLibraryLayout->insertWidget(
						MIN(MAX(i+d, 0), glassColorLibraryLayout->count()), w->widget()); 
					return;
				}
			}	
			return;	
		case PULLPLAN_VIEW_MODE:
			for (int i = 0; i < caneLibraryLayout->count(); ++i)
			{
				QLayoutItem* w = caneLibraryLayout->itemAt(i);
				Cane* p = dynamic_cast<CaneLibraryWidget*>(w->widget())->cane;
				if (p == caneEditorWidget->cane())
				{
					w = caneLibraryLayout->takeAt(i);
					caneLibraryLayout->insertWidget(
						MIN(MAX(i+d, 0), caneLibraryLayout->count()), w->widget()); 
					return;
				}
			}
			return;	
		case PIECE_VIEW_MODE:
			for (int i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				QLayoutItem* w = pieceLibraryLayout->itemAt(i);
				Piece* p = dynamic_cast<PieceLibraryWidget*>(w->widget())->piece;
				if (p == pieceEditorWidget->piece())
				{
					w = pieceLibraryLayout->takeAt(i);
					pieceLibraryLayout->insertWidget(
						MIN(MAX(i+d, 0), pieceLibraryLayout->count()), w->widget()); 
					return;
				}
			}
			return;	
	}

}

void MainWindow :: deleteCurrentEditingObject()
{
	GlassColor* currentColor = glassColorEditorWidget->glassColor();
	Cane* currentCane = caneEditorWidget->cane();
	Piece* currentPiece = pieceEditorWidget->piece();

	QVBoxLayout* libraryLayout = NULL;
	switch (editorStack->currentIndex())
	{
		case COLORBAR_VIEW_MODE:
			libraryLayout = glassColorLibraryLayout;
			break;
		case PULLPLAN_VIEW_MODE:
			libraryLayout = caneLibraryLayout;
			break;
		case PIECE_VIEW_MODE:
			libraryLayout = pieceLibraryLayout;
			break;
		default:
			return;
	}

	for (int i = 0; i < libraryLayout->count(); ++i)
	{
		QLayoutItem* oldCur = libraryLayout->itemAt(i);
		GlassLibraryWidget* glw = dynamic_cast<GlassLibraryWidget*>(oldCur->widget());
		switch (editorStack->currentIndex())
		{
			case COLORBAR_VIEW_MODE:
				if (currentColor == dynamic_cast<GlassColorLibraryWidget*>(glw)->glassColor)
					deleteLibraryWidget(glw);
				break;
			case PULLPLAN_VIEW_MODE:	
				if (currentCane == dynamic_cast<CaneLibraryWidget*>(glw)->cane)
					deleteLibraryWidget(glw);
				break;
			case PIECE_VIEW_MODE:	
				if (currentPiece == dynamic_cast<PieceLibraryWidget*>(glw)->piece)
					deleteLibraryWidget(glw);
				break;
		}	
	}	
}

bool MainWindow::findLibraryWidgetData(GlassLibraryWidget* lw, int* type, QVBoxLayout** layout, int* index)
{
	GlassColorLibraryWidget* cblw = dynamic_cast<GlassColorLibraryWidget*>(lw);
	CaneLibraryWidget* plplw = dynamic_cast<CaneLibraryWidget*>(lw);
	PieceLibraryWidget* plw = dynamic_cast<PieceLibraryWidget*>(lw);

	if (cblw != NULL)
	{
		if (cblw->glassColor != glassColorEditorWidget->glassColor())
		{
			setEditorLibraryWidget(lw);
			return false;
		}
		*type = COLORBAR_VIEW_MODE;
	}
	else if (plplw != NULL)
	{
		if (plplw->cane != caneEditorWidget->cane())
		{
			setEditorLibraryWidget(lw);
			return false;
		}
		*type = PULLPLAN_VIEW_MODE;
	}
	else if (plw != NULL)
	{
		if (plw->piece != pieceEditorWidget->piece())
		{
			setEditorLibraryWidget(lw);
			return false;
		}
		*type = PIECE_VIEW_MODE;
	}
	else
		return false;	

	*layout = NULL;
	switch (*type)
	{
		case COLORBAR_VIEW_MODE:
			*layout = glassColorLibraryLayout;
			break;
		case PULLPLAN_VIEW_MODE:
			*layout = caneLibraryLayout;
			break;
		case PIECE_VIEW_MODE:
			*layout = pieceLibraryLayout;
			break;
	}
	
	QLayoutItem* cur;
	for (*index = 0; *index < (*layout)->count(); ++(*index))
	{
		cur = (*layout)->itemAt((*index));
		if (cur->widget() == lw)
		{
			break;
		}
	}

	return true;
} 

void MainWindow::copyLibraryWidget(GlassLibraryWidget* lw)
{
	int typeCase;
	QVBoxLayout* layout;
	int index;

	if (!findLibraryWidgetData(lw, &typeCase, &layout, &index))
		return;
	
	switch (typeCase)
	{
		case COLORBAR_VIEW_MODE:	
		{
			GlassColor* newEditorGlassColor = glassColorEditorWidget->glassColor()->copy();
			glassColorLibraryLayout->insertWidget(index, 
				new GlassColorLibraryWidget(newEditorGlassColor, this));
			glassColorEditorWidget->setGlassColor(newEditorGlassColor);
			break;
		}
		case PULLPLAN_VIEW_MODE:
		{
			Cane *newEditorCane = caneEditorWidget->cane()->copy();
			caneLibraryLayout->insertWidget(index, 
				new CaneLibraryWidget(newEditorCane, this));
			caneEditorWidget->setCane(newEditorCane);
			break;
		}
		case PIECE_VIEW_MODE:
		{
			Piece* newEditorPiece = pieceEditorWidget->piece()->copy();
			pieceLibraryLayout->insertWidget(index, 
				new PieceLibraryWidget(newEditorPiece, this));
			pieceEditorWidget->setPiece(newEditorPiece);
			break;
		}
	}
	updateLibrary();

	setDirtyBit(true);
}

bool MainWindow::hasNoDependancies(GlassLibraryWidget* lw)
{
	GlassColorLibraryWidget* cblw = dynamic_cast<GlassColorLibraryWidget*>(lw);
	CaneLibraryWidget* plplw = dynamic_cast<CaneLibraryWidget*>(lw);

	if (cblw != NULL)
		return !glassColorIsDependancy(cblw->glassColor);
	else if (plplw != NULL)
		return !caneIsDependancy(plplw->cane);
	else
		return true;
}


void MainWindow::deleteLibraryWidget(GlassLibraryWidget* lw)
{
	int typeCase;
	QVBoxLayout* layout;
	int index;

	if (!hasNoDependancies(lw))
	{
		QMessageBox::warning(this, "Delete failed", 
			"This glass cannot be deleted:\nother objects use it.");
		return;
	}

	if (!findLibraryWidgetData(lw, &typeCase, &layout, &index))
		return;

	if (layout->count() > 1)
	{
		int r = (index == 0) ? 1 : index-1;
		switch (typeCase)
		{
			case COLORBAR_VIEW_MODE:
			{
				GlassColor* replacement = dynamic_cast<GlassColorLibraryWidget*>(
					dynamic_cast<QWidgetItem*>(layout->itemAt(r))->widget())->glassColor;
				glassColorEditorWidget->setGlassColor(replacement);
				break;	
			}
			case PULLPLAN_VIEW_MODE:
			{
				Cane* replacement = dynamic_cast<CaneLibraryWidget*>(
					dynamic_cast<QWidgetItem*>(layout->itemAt(r))->widget())->cane;
				caneEditorWidget->setCane(replacement);
				break;	
			}
			case PIECE_VIEW_MODE:
			{
				Piece* replacement = dynamic_cast<PieceLibraryWidget*>(
					dynamic_cast<QWidgetItem*>(layout->itemAt(r))->widget())->piece;
				pieceEditorWidget->setPiece(replacement);
				break;	
			}
		}
	}
	else
		setViewMode(EMPTY_VIEW_MODE);

	QLayoutItem* cur = layout->takeAt(index);
	cur->widget()->moveToThread(QApplication::instance()->thread());
	cur->widget()->deleteLater();
	delete cur;
	// this is a memory leak, as the glass color/cane/piece is never deleted
	updateLibrary();

	setDirtyBit(true);
}

void MainWindow::setEditorLibraryWidget(GlassLibraryWidget* w)
{
	GlassColorLibraryWidget* cblw = dynamic_cast<GlassColorLibraryWidget*>(w);
	CaneLibraryWidget* plplw = dynamic_cast<CaneLibraryWidget*>(w);
	PieceLibraryWidget* plw = dynamic_cast<PieceLibraryWidget*>(w);

	if (cblw != NULL)
	{
		setViewMode(COLORBAR_VIEW_MODE);
		glassColorEditorWidget->setGlassColor(cblw->glassColor);
		updateLibrary();
	}
	else if (plplw != NULL)
	{
		setViewMode(PULLPLAN_VIEW_MODE);
		caneEditorWidget->setCane(plplw->cane);
		updateLibrary();
	}
	else if (plw != NULL)
	{
		setViewMode(PIECE_VIEW_MODE);
		pieceEditorWidget->setPiece(plw->piece);
		updateLibrary();
	}
}

void MainWindow :: setupConnections()
{
	// Editor "stuff changing" communication
	connect(glassColorEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateLibrary()));
	connect(glassColorEditorWidget, SIGNAL(someDataChanged()), this, SLOT(setDirtyBitTrue()));

	connect(caneEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateLibrary()));
	connect(caneEditorWidget, SIGNAL(someDataChanged()), this, SLOT(setDirtyBitTrue()));

	connect(pieceEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateLibrary()));
	connect(pieceEditorWidget, SIGNAL(someDataChanged()), this, SLOT(setDirtyBitTrue()));

	// Toolbar stuff
	connect(newFileButton, SIGNAL(clicked()), this, SLOT(newFileActionTriggered()));
	connect(openFileButton, SIGNAL(clicked()), this, SLOT(openFileActionTriggered()));
	connect(saveFileButton, SIGNAL(clicked()), this, SLOT(saveAllFileActionTriggered()));
	connect(shareFileButton, SIGNAL(clicked()), this, SLOT(shareFileActionTriggered()));
	connect(email, SIGNAL(success(QString)), this, SLOT(emailSuccess(QString)));
	connect(email, SIGNAL(failure(QString)), this, SLOT(emailFailure(QString)));

	// Library stuff
	connect(newGlassColorButton, SIGNAL(clicked()), this, SLOT(newGlassColorButtonClicked()));
	connect(newCaneButton, SIGNAL(clicked()), this, SLOT(newCaneButtonClicked()));
	connect(newPieceButton, SIGNAL(clicked()), this, SLOT(newPieceButtonClicked()));

	// File menu stuff
	connect(newFileAction, SIGNAL(triggered()), this, SLOT(newFileActionTriggered()));
	connect(openFileAction, SIGNAL(triggered()), this, SLOT(openFileActionTriggered()));
	connect(addFileAction, SIGNAL(triggered()), this, SLOT(addFileActionTriggered()));
	connect(importSVGFileAction, SIGNAL(triggered()), this, SLOT(importSVGActionTriggered()));
	connect(exportPLYFileAction, SIGNAL(triggered()), this, SLOT(exportPLYActionTriggered()));
	connect(exportOBJFileAction, SIGNAL(triggered()), this, SLOT(exportOBJActionTriggered()));
	connect(saveAllFileAction, SIGNAL(triggered()), this, SLOT(saveAllFileActionTriggered()));
	connect(saveAllAsFileAction, SIGNAL(triggered()), this, SLOT(saveAllAsFileActionTriggered()));
	connect(saveSelectedAsFileAction, SIGNAL(triggered()), this, SLOT(saveSelectedAsFileActionTriggered()));
	connect(exitAction, SIGNAL(triggered()), this, SLOT(attemptToQuit()));

	// Edit menu stuff
	connect(undoAction, SIGNAL(triggered()), this, SLOT(undoActionTriggered()));
	connect(redoAction, SIGNAL(triggered()), this, SLOT(redoActionTriggered()));
	
	// View menu stuff
	connect(fullscreenViewAction, SIGNAL(triggered()), this, SLOT(fullscreenViewActionTriggered()));
	connect(windowedViewAction, SIGNAL(triggered()), this, SLOT(windowedViewActionTriggered()));

	// Examples menu stuff
	connect(randomSimpleCaneAction, SIGNAL(triggered()), this, SLOT(randomSimpleCaneExampleActionTriggered()));
	connect(randomSimplePieceAction, SIGNAL(triggered()), this, SLOT(randomSimplePieceExampleActionTriggered()));

	connect(randomComplexCaneAction, SIGNAL(triggered()), this, SLOT(randomComplexCaneExampleActionTriggered()));
	connect(randomComplexPieceAction, SIGNAL(triggered()), this, SLOT(randomComplexPieceExampleActionTriggered()));

	// Performance menu stuff
	connect(depthPeelAction, SIGNAL(triggered()), this, SLOT(depthPeelActionTriggered()));

	// Status bar stuff
	connect(caneEditorWidget, SIGNAL(showMessage(const QString&, unsigned int)), 
		this, SLOT(showStatusMessage(const QString&, unsigned int)));
	connect(pieceEditorWidget, SIGNAL(showMessage(const QString&, unsigned int)), 
		this, SLOT(showStatusMessage(const QString&, unsigned int)));
	connect(email, SIGNAL(showMessage(const QString&, unsigned int)), 
		this, SLOT(showStatusMessage(const QString&, unsigned int)));
}

void MainWindow :: undoActionTriggered()
{
	switch (editorStack->currentIndex())
	{
		case COLORBAR_VIEW_MODE:
			glassColorEditorWidget->undo();
			break;
		case PULLPLAN_VIEW_MODE:
			caneEditorWidget->undo();
			break;
		case PIECE_VIEW_MODE:
			pieceEditorWidget->undo();
			break;
	}
}

void MainWindow :: redoActionTriggered()
{
	switch (editorStack->currentIndex())
	{
		case COLORBAR_VIEW_MODE:
			glassColorEditorWidget->redo();
			break;
		case PULLPLAN_VIEW_MODE:
			caneEditorWidget->redo();
			break;
		case PIECE_VIEW_MODE:
			pieceEditorWidget->redo();
			break;
	}
}

void MainWindow :: fullscreenViewActionTriggered()
{
	showFullScreen();
	fullscreenViewAction->setChecked(true);
	windowedViewAction->setChecked(false);
}

void MainWindow :: windowedViewActionTriggered()
{
	showMaximized();
	fullscreenViewAction->setChecked(false);
	windowedViewAction->setChecked(true);
}

void MainWindow :: depthPeelActionTriggered()
{
	GlobalDepthPeelingSetting::setEnabled(!GlobalDepthPeelingSetting::enabled());
	depthPeelAction->setChecked(GlobalDepthPeelingSetting::enabled());

	// lazily redraw only the current editor's view(s)
	switch (editorStack->currentIndex())
	{
		case COLORBAR_VIEW_MODE:
			glassColorEditorWidget->updateEverything();
			break;
		case PULLPLAN_VIEW_MODE:
			caneEditorWidget->updateEverything();
			break;
		case PIECE_VIEW_MODE:
			pieceEditorWidget->updateEverything();
			break;
	}

	// redraw all library thumbnails
	for (int i = 0; i < glassColorLibraryLayout->count(); ++i)
	{
		dynamic_cast<GlassColorLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(glassColorLibraryLayout->itemAt(i))->widget())->updatePixmaps();
	}
	for (int i = 0; i < caneLibraryLayout->count(); ++i)
	{
		dynamic_cast<CaneLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(caneLibraryLayout->itemAt(i))->widget())->updatePixmaps();
	}
	for (int i = 0; i < pieceLibraryLayout->count(); ++i)
	{
		dynamic_cast<PieceLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget())->updatePixmap();
	}
}

void MainWindow :: randomSimpleCaneExampleActionTriggered()
{
	GlassColor* randomGC = randomGlassColor();
	Cane* randomPP = randomSimpleCane(CIRCLE_SHAPE, randomGC);

	glassColorLibraryLayout->addWidget(new GlassColorLibraryWidget(randomGC, this));
	setDirtyBit(true);

	CaneLibraryWidget* pplw = new CaneLibraryWidget(randomPP, this);
	caneLibraryLayout->addWidget(pplw);

	setViewMode(PULLPLAN_VIEW_MODE);
	caneEditorWidget->setCane(randomPP);
	updateLibrary();
}

void MainWindow :: randomComplexCaneExampleActionTriggered()
{
	GlassColor* randomGC = randomGlassColor();
	Cane* randomCPP = randomSimpleCane(CIRCLE_SHAPE, randomGC);
	Cane* randomSPP = randomSimpleCane(SQUARE_SHAPE, randomGC);
	Cane* randomComplexPP = randomComplexCane(randomCPP, randomSPP);

	glassColorLibraryLayout->addWidget(new GlassColorLibraryWidget(randomGC, this));
	caneLibraryLayout->addWidget(new CaneLibraryWidget(randomComplexPP, this));
	setDirtyBit(true);

	// add simple canes only if they are used
	// memory leak! as unused ones never appear in library
	if (randomComplexPP->hasDependencyOn(randomCPP))
		caneLibraryLayout->addWidget(new CaneLibraryWidget(randomCPP, this));
	if (randomComplexPP->hasDependencyOn(randomSPP))
		caneLibraryLayout->addWidget(new CaneLibraryWidget(randomSPP, this));

	setViewMode(PULLPLAN_VIEW_MODE);
	caneEditorWidget->setCane(randomComplexPP);
	updateLibrary();
}

void MainWindow :: randomSimplePieceExampleActionTriggered()
{
	GlassColor* randomGC = randomGlassColor();
	Cane* randomSPP = randomSimpleCane(SQUARE_SHAPE, randomGC);
	Piece* randomP = randomPiece(randomPickup(randomSPP));

	glassColorLibraryLayout->addWidget(new GlassColorLibraryWidget(randomGC, this));
	caneLibraryLayout->addWidget(new CaneLibraryWidget(randomSPP, this));
	setDirtyBit(true);

	PieceLibraryWidget* plw = new PieceLibraryWidget(randomP, this);
	pieceLibraryLayout->addWidget(plw);

	setViewMode(PIECE_VIEW_MODE);
	pieceEditorWidget->setPiece(randomP);
	updateLibrary();
}

void MainWindow :: randomComplexPieceExampleActionTriggered()
{
	GlassColor* randomGC1 = randomGlassColor();
	GlassColor* randomGC2 = randomGlassColor();
	Cane* randomCPP = randomSimpleCane(CIRCLE_SHAPE, randomGC1);
	Cane* randomSPP = randomSimpleCane(SQUARE_SHAPE, randomGC2);
	Cane* randomComplexPP1 = randomComplexCane(randomCPP, randomSPP);
	Cane* randomComplexPP2 = randomComplexCane(randomCPP, randomSPP);
	Piece* randomP = randomPiece(randomPickup(randomComplexPP1, randomComplexPP2));

	if (randomP->hasDependencyOn(randomGC1)) // memory leak if returns no
		glassColorLibraryLayout->addWidget(new GlassColorLibraryWidget(randomGC1, this));
	if (randomP->hasDependencyOn(randomGC2)) // memory leak if returns no
		glassColorLibraryLayout->addWidget(new GlassColorLibraryWidget(randomGC2, this));
	if (randomP->hasDependencyOn(randomCPP)) // memory leak if returns no
		caneLibraryLayout->addWidget(new CaneLibraryWidget(randomCPP, this));
	if (randomP->hasDependencyOn(randomSPP)) // memory leak if returns no
		caneLibraryLayout->addWidget(new CaneLibraryWidget(randomSPP, this));
	caneLibraryLayout->addWidget(new CaneLibraryWidget(randomComplexPP1, this));
	caneLibraryLayout->addWidget(new CaneLibraryWidget(randomComplexPP2, this));
	pieceLibraryLayout->addWidget(new PieceLibraryWidget(randomP, this));
	setDirtyBit(true);

	setViewMode(PIECE_VIEW_MODE);
	pieceEditorWidget->setPiece(randomP);
	updateLibrary();
}

void MainWindow :: setDirtyBitTrue()
{
	setDirtyBit(true);
}

void MainWindow :: setDirtyBit(bool v)
{
	dirtyBit = v;
	setWindowTitle(windowTitle());
}

void MainWindow :: setSaveFilename(QString filename)
{
	saveFilename = filename;
	setWindowTitle(windowTitle());
}

void MainWindow :: setupSaveFile()
{
	// going to use [unsaved] as a reserved token implying an uninitialized save file.
	// if you try to save to a file named [unsaved], there will be no mercy.
	dirtyBit = false;
	saveFilename = "[unsaved]"; 
}

void MainWindow :: setupLibrary()
{
	QWidget* libraryMasterWidget = new QWidget(centralWidget);
	centralLayout->addWidget(libraryMasterWidget);

	QVBoxLayout* libraryAreaLayout = new QVBoxLayout(libraryMasterWidget);
	libraryMasterWidget->setLayout(libraryAreaLayout);
	libraryAreaLayout->setContentsMargins(0, 0, 0, 0);

	// Toolbar stuff

	QWidget* toolbarMasterWidget = new QWidget(libraryMasterWidget);
	libraryAreaLayout->addWidget(toolbarMasterWidget);

	QHBoxLayout* toolbarLayout = new QHBoxLayout(toolbarMasterWidget);
	toolbarMasterWidget->setLayout(toolbarLayout);
	toolbarLayout->setContentsMargins(0, 0, 0, 0);

	newFileButton = new QPushButton(toolbarMasterWidget);
	newFileButton->setText("New");
	toolbarLayout->addWidget(newFileButton);

	openFileButton = new QPushButton(toolbarMasterWidget);
	openFileButton->setText("Open");
	toolbarLayout->addWidget(openFileButton);

	saveFileButton = new QPushButton(toolbarMasterWidget);
	saveFileButton->setText("Save");
	toolbarLayout->addWidget(saveFileButton);

	shareFileButton = new QPushButton(toolbarMasterWidget);
	shareFileButton->setText("Email");
	toolbarLayout->addWidget(shareFileButton);
	email = new Email();

	// The actual library
	
	libraryScrollArea = new QScrollArea(libraryMasterWidget);
	libraryAreaLayout->addWidget(libraryScrollArea, 1);
	// Filter out up/down arrow key events for moving library objects around.
	// See eventFilter() for details. 
	libraryScrollArea->installEventFilter(this);	
	libraryScrollArea->setBackgroundRole(QPalette::Dark);
	libraryScrollArea->setWidgetResizable(true);
	libraryScrollArea->setFixedWidth(358);
	libraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	libraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	QWidget* libraryWidget = new QWidget(libraryScrollArea);
	libraryScrollArea->setWidget(libraryWidget);

	// TODO: refactor library to only use one layout
	QGridLayout* superlibraryLayout = new QGridLayout(libraryWidget);
	libraryWidget->setLayout(superlibraryLayout);
	superlibraryLayout->setContentsMargins(10, 10, 10, 10);
	superlibraryLayout->setSpacing(10);

	superlibraryLayout->addItem(new QSpacerItem(100, 0), 0, 0);
	newGlassColorButton = new QPushButton("New Color", libraryWidget);
	superlibraryLayout->addWidget(newGlassColorButton, 1, 0);

	superlibraryLayout->addItem(new QSpacerItem(100, 0), 0, 1);
	newCaneButton = new QPushButton("New Cane", libraryWidget);
	superlibraryLayout->addWidget(newCaneButton, 1, 1);

	superlibraryLayout->addItem(new QSpacerItem(100, 0), 0, 2);
	newPieceButton = new QPushButton("New Piece", libraryWidget);
	superlibraryLayout->addWidget(newPieceButton, 1, 2);

	glassColorLibraryLayout = new QVBoxLayout(libraryWidget);
	glassColorLibraryLayout->setSpacing(10);
	glassColorLibraryLayout->setDirection(QBoxLayout::BottomToTop);
	caneLibraryLayout = new QVBoxLayout(libraryWidget);
	caneLibraryLayout->setSpacing(10);
	caneLibraryLayout->setDirection(QBoxLayout::BottomToTop);
	pieceLibraryLayout = new QVBoxLayout(libraryWidget);
	pieceLibraryLayout->setSpacing(10);
	pieceLibraryLayout->setDirection(QBoxLayout::BottomToTop);
	superlibraryLayout->addLayout(glassColorLibraryLayout, 2, 0, Qt::AlignTop);
	superlibraryLayout->addLayout(caneLibraryLayout, 2, 1, Qt::AlignTop);
	superlibraryLayout->addLayout(pieceLibraryLayout, 2, 2, Qt::AlignTop);
	
	superlibraryLayout->setColumnStretch(3, 1);

	// make three qlabels for a legend
	QWidget* legendWidget = new QWidget(libraryMasterWidget);
	legendWidget->setMinimumHeight(50); // to match alphaWidget in ColorEditorWidget
	libraryAreaLayout->addWidget(legendWidget);

	QGridLayout* legendLayout = new QGridLayout(legendWidget);
	legendWidget->setLayout(legendLayout);
	legendLayout->setColumnStretch(0, 1);
	legendLayout->setColumnStretch(1, 1);
	legendLayout->setColumnStretch(2, 1);
	legendLayout->setContentsMargins(0, 0, 0, 0);

	QLabel* l1 = new QLabel("Used By Selected", legendWidget);
	l1->setStyleSheet("border: 2px dashed " + QColor(0, 139, 69, 255).name() + ";");
	legendLayout->addWidget(l1, 0, 0, Qt::AlignCenter);
	QLabel* l2 = new QLabel("  Selected  ", legendWidget);
	l2->setStyleSheet("border: 2px solid " + QColor(0, 0, 255, 255).name() + ";");
	legendLayout->addWidget(l2, 0, 1, Qt::AlignCenter);
	QLabel* l3 = new QLabel("Uses Selected", legendWidget);
	l3->setStyleSheet("border: 2px dotted " + QColor(200, 100, 0, 255).name() + ";");
	legendLayout->addWidget(l3, 0, 2, Qt::AlignCenter);

	QLabel* descriptionLabel = new QLabel("Library - click to edit or drag to add.", legendWidget);
	legendLayout->addWidget(descriptionLabel, 1, 0, 1, 3, Qt::AlignCenter);
}

void MainWindow :: clearLibrary()
{
	// TODO: fix the memory leak here, as we delete
	// all of the library objects but none of the glass objects they represent
	// the non-trivial part is just the editors and their "current editing object"
	QLayoutItem* w;
	
	while (glassColorLibraryLayout->count() > 0)
	{
		w = glassColorLibraryLayout->takeAt(0);
		delete w->widget();
		delete w;
	}
	while (caneLibraryLayout->count() > 0)
	{
		w = caneLibraryLayout->takeAt(0);
		delete w->widget();
		delete w;
	}
	while (pieceLibraryLayout->count() > 0)
	{
		w = pieceLibraryLayout->takeAt(0);
		delete w->widget();
		delete w;
	}
}

void MainWindow :: setupEditors()
{
	editorStack = new QStackedWidget(centralWidget);
	centralLayout->addWidget(editorStack, 2);

	// The order that the editors are added to the stacked widget
	// must match their order values in the enum EditorGUI::Mode
	// in mainwindow.h

	// "null" editor 
	QLabel* whatToDoLabel = new QLabel("Click a library item at left to edit/view.", editorStack);
	whatToDoLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	editorStack->addWidget(whatToDoLabel);

	// real editors
	setupColorEditor();
	editorStack->addWidget(glassColorEditorWidget);

	setupCaneEditor();
	editorStack->addWidget(caneEditorWidget);

	setupPieceEditor();
	editorStack->addWidget(pieceEditorWidget);
}

void MainWindow :: setupColorEditor()
{
	// Setup data objects - the current cane and library widget for this cane
	glassColorEditorWidget = new ColorEditorWidget(editorStack);
	glassColorLibraryLayout->addWidget(
		new GlassColorLibraryWidget(glassColorEditorWidget->glassColor(), this));
	glassColorEditorWidget->updateEverything();
}

void MainWindow :: setupCaneEditor()
{
	// Setup data objects - the current cane and library widget for this cane
	caneEditorWidget = new CaneEditorWidget(editorStack);
	caneLibraryLayout->addWidget(
		new CaneLibraryWidget(caneEditorWidget->cane(), this));
	caneEditorWidget->updateEverything();
}

void MainWindow :: setupPieceEditor()
{
	// Setup data objects - the current cane and library widget for this cane
	pieceEditorWidget = new PieceEditorWidget(editorStack);
	pieceLibraryLayout->addWidget(new PieceLibraryWidget(pieceEditorWidget->piece(), this));
	pieceEditorWidget->updateEverything();
}

void MainWindow :: newGlassColorButtonClicked()
{
	GlassColor* newGlassColor = new GlassColor();
	glassColorLibraryLayout->addWidget(new GlassColorLibraryWidget(newGlassColor, this));
	setViewMode(COLORBAR_VIEW_MODE);
	glassColorEditorWidget->setGlassColor(newGlassColor);
	setDirtyBit(true);
	updateLibrary();
}

void MainWindow :: newCaneButtonClicked()
{
	Cane *newEditorCane = new Cane(PullTemplate::HORIZONTAL_LINE_CIRCLE);
	caneLibraryLayout->addWidget(new CaneLibraryWidget(newEditorCane, this));
	setViewMode(PULLPLAN_VIEW_MODE);
	caneEditorWidget->setCane(newEditorCane);
	setDirtyBit(true);
	updateLibrary();
}

void MainWindow :: newPieceButtonClicked()
{
	Piece* newEditorPiece = new Piece(PieceTemplate::TUMBLER);
	pieceLibraryLayout->addWidget(new PieceLibraryWidget(newEditorPiece, this));
	setViewMode(PIECE_VIEW_MODE);
	pieceEditorWidget->setPiece(newEditorPiece);
	setDirtyBit(true);
	updateLibrary();
}

// returns whether the cane is a dependancy of something in the library
// (either a cane or a piece)
bool MainWindow :: glassColorIsDependancy(GlassColor* color)
{
	CaneLibraryWidget* pplw;
	for (int i = 0; i < caneLibraryLayout->count(); ++i)
	{
		pplw = dynamic_cast<CaneLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(caneLibraryLayout->itemAt(i))->widget());
		if (pplw->cane->hasDependencyOn(color))
		{
			return true;
		}
	}

	PieceLibraryWidget* plw;
	for (int i = 0; i < pieceLibraryLayout->count(); ++i)
	{
		plw = dynamic_cast<PieceLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
		if (plw->piece->hasDependencyOn(color))
		{
			return true;
		}
	}

	return false;
}

// returns whether the cane is a dependancy of something in the library
// (either another cane or a piece)
bool MainWindow :: caneIsDependancy(Cane* cane)
{
	CaneLibraryWidget* pplw;
	for (int i = 0; i < caneLibraryLayout->count(); ++i)
	{
		pplw = dynamic_cast<CaneLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(caneLibraryLayout->itemAt(i))->widget());
		// Check whether the cane in the library is:
		// 1. the parameter cane
		// 2. a cane with the cane currently being edited as a subcane
		if (pplw->cane == cane)
		{
			continue;
		}
		else if (pplw->cane->hasDependencyOn(cane))
		{
			return true;
		}
	}

	PieceLibraryWidget* plw;
	for (int i = 0; i < pieceLibraryLayout->count(); ++i)
	{
		plw = dynamic_cast<PieceLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
		if (plw->piece->hasDependencyOn(cane))
		{
			return true;
		}
	}

	return false;
}

void MainWindow :: updateLibrary()
{
	switch (editorStack->currentIndex())
	{
		case EMPTY_VIEW_MODE:
		{
			undoAction->setEnabled(false);
			redoAction->setEnabled(false);
			for (int i = 0; i < glassColorLibraryLayout->count(); ++i)
			{
				GlassColorLibraryWidget* cblw = dynamic_cast<GlassColorLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(glassColorLibraryLayout->itemAt(i))->widget());
				cblw->setDependancy(NO_DEPENDANCY);
			}
			for (int i = 0; i < caneLibraryLayout->count(); ++i)
			{
				CaneLibraryWidget* pplw = dynamic_cast<CaneLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(caneLibraryLayout->itemAt(i))->widget());
				pplw->setDependancy(NO_DEPENDANCY);
			}
			for (int i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				PieceLibraryWidget* plw = dynamic_cast<PieceLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
				plw->setDependancy(NO_DEPENDANCY);
			}
			break;
		}
		case COLORBAR_VIEW_MODE:
		{
			undoAction->setEnabled(glassColorEditorWidget->canUndo());
			redoAction->setEnabled(glassColorEditorWidget->canRedo());
			for (int i = 0; i < glassColorLibraryLayout->count(); ++i)
			{
				GlassColorLibraryWidget* cblw = dynamic_cast<GlassColorLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(glassColorLibraryLayout->itemAt(i))->widget());
				if (glassColorEditorWidget->glassColor() == cblw->glassColor)
				{
					cblw->updatePixmaps();
					cblw->setDependancy(IS_DEPENDANCY);
				}
				else
					cblw->setDependancy(NO_DEPENDANCY);
			}

			for (int i = 0; i < caneLibraryLayout->count(); ++i)
			{
				CaneLibraryWidget* pplw = dynamic_cast<CaneLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(caneLibraryLayout->itemAt(i))->widget());
				if (pplw->cane->hasDependencyOn(glassColorEditorWidget->glassColor()))
				{
					pplw->updatePixmaps();
					pplw->setDependancy(USES_DEPENDANCY);
				}
				else
					pplw->setDependancy(NO_DEPENDANCY);
			}

			for (int i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				PieceLibraryWidget* plw = dynamic_cast<PieceLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
				if (plw->piece->hasDependencyOn(glassColorEditorWidget->glassColor()))
				{
					plw->updatePixmap();
					plw->setDependancy(USES_DEPENDANCY);
				}
				else
					plw->setDependancy(NO_DEPENDANCY);
			}

			break;
		}
		case PULLPLAN_VIEW_MODE:
		{
			undoAction->setEnabled(caneEditorWidget->canUndo());
			redoAction->setEnabled(caneEditorWidget->canRedo());
			for (int i = 0; i < glassColorLibraryLayout->count(); ++i)
			{
				GlassColorLibraryWidget* cblw = dynamic_cast<GlassColorLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(glassColorLibraryLayout->itemAt(i))->widget());
				if (caneEditorWidget->cane()->hasDependencyOn(cblw->glassColor))
					cblw->setDependancy(USEDBY_DEPENDANCY);
				else
					cblw->setDependancy(NO_DEPENDANCY);
			}
			for (int i = 0; i < caneLibraryLayout->count(); ++i)
			{
				CaneLibraryWidget* pplw = dynamic_cast<CaneLibraryWidget*>(
						dynamic_cast<QWidgetItem *>(caneLibraryLayout->itemAt(i))->widget());
				// Check whether the cane in the library is:
				// 1. the cane currently being edited
				// 2. a subcane of the cane current being edited
				// 3. a cane with the cane currently being edited as a subcane
				if (caneEditorWidget->cane() == pplw->cane)
				{
					pplw->updatePixmaps();
					pplw->setDependancy(IS_DEPENDANCY);
				}
				else if (caneEditorWidget->cane()->hasDependencyOn(pplw->cane))
				{
					pplw->setDependancy(USEDBY_DEPENDANCY);
				}
				else if (pplw->cane->hasDependencyOn(caneEditorWidget->cane()))
				{
					pplw->updatePixmaps();
					pplw->setDependancy(USES_DEPENDANCY);
				}
				else
					pplw->setDependancy(NO_DEPENDANCY);
			}
			for (int i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				PieceLibraryWidget* plw = dynamic_cast<PieceLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
				if (plw->piece->hasDependencyOn(caneEditorWidget->cane()))
				{
					plw->updatePixmap();
					plw->setDependancy(USES_DEPENDANCY);
				}
				else
					plw->setDependancy(NO_DEPENDANCY);
			}

			break;
		}
		case PIECE_VIEW_MODE:
		{
			undoAction->setEnabled(pieceEditorWidget->canUndo());
			redoAction->setEnabled(pieceEditorWidget->canRedo());
			for (int i = 0; i < glassColorLibraryLayout->count(); ++i)
			{
				GlassColorLibraryWidget* cblw = dynamic_cast<GlassColorLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(glassColorLibraryLayout->itemAt(i))->widget());
				if (pieceEditorWidget->piece()->hasDependencyOn(cblw->glassColor))
					cblw->setDependancy(USEDBY_DEPENDANCY);
				else
					cblw->setDependancy(NO_DEPENDANCY);
			}
			for (int i = 0; i < caneLibraryLayout->count(); ++i)
			{
				CaneLibraryWidget* pplw = dynamic_cast<CaneLibraryWidget*>(
					dynamic_cast<QWidgetItem*>(caneLibraryLayout->itemAt(i))->widget());
				if (pieceEditorWidget->piece()->hasDependencyOn(pplw->cane))
					pplw->setDependancy(USEDBY_DEPENDANCY);
				else
					pplw->setDependancy(NO_DEPENDANCY);
			}
			for (int i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				PieceLibraryWidget* plw = dynamic_cast<PieceLibraryWidget*>(
						dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
				if (plw->piece == pieceEditorWidget->piece())
				{
					plw->updatePixmap();
					plw->setDependancy(IS_DEPENDANCY);
				}
				else
					plw->setDependancy(NO_DEPENDANCY);
			}

			break;
		}
	}
}

void MainWindow::closeEvent(QCloseEvent * event)
{
	attemptToQuit();	
	event->ignore(); // if you didn't quit, don't
}

void MainWindow::attemptToQuit()
{
	if (!dirtyBit)
	{
		QCoreApplication::exit();
		return;
	}

	// setup and launch the warning dialog
	// using http://qt-project.org/doc/qt-4.8/qmessagebox.html#the-property-based-api
	// and http://developer.apple.com/library/mac/documentation/UserExperience/Conceptual/AppleHIGuidelines/Windows/Windows.html#//apple_ref/doc/uid/20000961-BABCAJID
	// as sources
	QMessageBox msgBox;
	msgBox.setText("The glass library has been modified.");
	msgBox.setInformativeText("Do you want to save your changes?");
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Save);
	int returnValue = msgBox.exec();

	switch (returnValue)
	{
		case QMessageBox::Cancel:
			return;
		case QMessageBox::Save:
			saveAllFileActionTriggered();
			break;
		case QMessageBox::Discard:
			break;
	}

	QCoreApplication::exit();
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);
	menu.exec(event->globalPos());
}

void MainWindow::setupMenus()
{
	// File menu
	fileMenu = menuBar()->addMenu("File"); 
	
	newFileAction = new QAction("New", this);
	newFileAction->setShortcuts(QKeySequence::New);
	newFileAction->setToolTip("Open a new empty file.");
	fileMenu->addAction(newFileAction); 
	
	openFileAction = new QAction("Open", this);
	openFileAction->setShortcuts(QKeySequence::Open);
	openFileAction->setToolTip("Open an existing file.");
	fileMenu->addAction(openFileAction); 
	
	addFileAction = new QAction("Add", this);
	addFileAction->setToolTip("Add an existing file.");
	fileMenu->addAction(addFileAction); 
	
	fileMenu->addSeparator();
	
	saveAllFileAction = new QAction("Save", this);
	saveAllFileAction->setShortcuts(QKeySequence::Save);
	saveAllFileAction->setToolTip("Save library to file.");
	fileMenu->addAction(saveAllFileAction); 

	saveAllAsFileAction = new QAction("Save As", this);
	saveAllAsFileAction->setShortcuts(QKeySequence::SaveAs);
	saveAllAsFileAction->setToolTip("Save library to file.");
	fileMenu->addAction(saveAllAsFileAction); 

	saveSelectedAsFileAction = new QAction("Save Selected As", this);
	saveSelectedAsFileAction->setToolTip("Save selected object to file.");
	fileMenu->addAction(saveSelectedAsFileAction); 

	importSVGFileAction = new QAction("Import cane from .svg", this);
	importSVGFileAction->setToolTip("Import cane cross section from .svg file.");
	fileMenu->addAction(importSVGFileAction); 

	exportPLYFileAction = new QAction("Export glass to .ply", this);
	exportPLYFileAction->setToolTip("Export cane or piece");
	fileMenu->addAction(exportPLYFileAction); 

	exportOBJFileAction = new QAction("Export glass to .obj", this);
	exportOBJFileAction->setToolTip("Export cane or piece");
	fileMenu->addAction(exportOBJFileAction); 

	fileMenu->addSeparator();

	exitAction = new QAction(tr("Quit"), this);
	exitAction->setShortcuts(QKeySequence::Quit);
	exitAction->setToolTip("Quit");
	fileMenu->addAction(exitAction);

	// Edit menu
	editMenu = menuBar()->addMenu("Edit");

	undoAction = new QAction("&Undo", this);	
	undoAction->setShortcut(QKeySequence::Undo);
	undoAction->setToolTip("Undo");
	editMenu->addAction(undoAction);

	redoAction = new QAction("&Redo", this);	
	redoAction->setShortcut(QKeySequence::Redo);
	redoAction->setToolTip("Redo");
	editMenu->addAction(redoAction);

	// View menu
	viewMenu = menuBar()->addMenu("View");

	fullscreenViewAction = new QAction("Full Screen", this);
	fullscreenViewAction->setCheckable(true);
	fullscreenViewAction->setChecked(false); 
	viewMenu->addAction(fullscreenViewAction);	

	windowedViewAction = new QAction("Windowed", this);
	windowedViewAction->setCheckable(true);
	windowedViewAction->setChecked(true); // start true b/c of showMaximized() call in constructor
	viewMenu->addAction(windowedViewAction);	

	// Examples menu
	examplesMenu = menuBar()->addMenu("Examples"); 

	randomSimpleCaneAction = new QAction("Simple Cane", this);
	randomSimpleCaneAction->setToolTip("Randomly generate a simple example cane.");
	examplesMenu->addAction(randomSimpleCaneAction);

	randomComplexCaneAction = new QAction("Complex Cane", this);
	randomComplexCaneAction->setToolTip("Ranomly generate a complex example cane.");
	examplesMenu->addAction(randomComplexCaneAction);

	examplesMenu->addSeparator();

	randomSimplePieceAction = new QAction("Simple Piece", this);
	randomSimplePieceAction->setToolTip("Randomly generate a simple example piece.");
	examplesMenu->addAction(randomSimplePieceAction);

	randomComplexPieceAction = new QAction("Complex Piece", this);
	randomComplexPieceAction->setToolTip("Randomly generate a complex example piece.");
	examplesMenu->addAction(randomComplexPieceAction);

	// Performance menu
	perfMenu = menuBar()->addMenu("Performance");

	depthPeelAction = new QAction("GPU Transparency", this);
	depthPeelAction->setCheckable(true);
	depthPeelAction->setChecked(GlobalDepthPeelingSetting::enabled());
	depthPeelAction->setToolTip(tr("Toggle transparency in 3D views. Turn off for better framerate."));
	perfMenu->addAction(depthPeelAction);
}

void MainWindow::exportOBJActionTriggered()
{
	// should never be invoked if menu disabling is working correctly, but just in case
	if (editorStack->currentIndex() == EMPTY_VIEW_MODE 
		|| editorStack->currentIndex() == COLORBAR_VIEW_MODE)
		return; 

	// get a filename from the user, suggesting "untitled.obj".
	QString userSpecifiedFilename = QFileDialog::getSaveFileName(this, "Save as...", 
		"untitled.obj", "Wavefront object file (*.obj)");

	// check that the user chose something
	if (userSpecifiedFilename.isNull())
		return;
	// if they didn't chose a .obj file, make it one
	if (!userSpecifiedFilename.endsWith(".obj"))
		userSpecifiedFilename += ".obj";

	// call it on currently selected object
	switch (editorStack->currentIndex())
	{
		case PULLPLAN_VIEW_MODE:
			caneEditorWidget->writeCaneToOBJFile(userSpecifiedFilename);
			return;	
		case PIECE_VIEW_MODE:
			pieceEditorWidget->writePieceToOBJFile(userSpecifiedFilename);
			return; 
		default: // should never get here as button should be disabled
			return;
	}	
}

void MainWindow::exportPLYActionTriggered()
{
	// get a filename from the user, suggesting "untitled.ply".
	QString userSpecifiedFilename = QFileDialog::getSaveFileName(this, "Save as...", 
		"untitled.ply", "Polygon file (*.ply)");

	// check that the user chose something
	if (userSpecifiedFilename.isNull())
		return;
	// if they didn't chose a .ply file, make it one
	if (!userSpecifiedFilename.endsWith(".ply"))
		userSpecifiedFilename += ".ply";

	// call it on currently selected object
	switch (editorStack->currentIndex())
	{
		case PULLPLAN_VIEW_MODE:
			caneEditorWidget->writeCaneToPLYFile(userSpecifiedFilename);
			return;	
		case PIECE_VIEW_MODE:
			pieceEditorWidget->writePieceToPLYFile(userSpecifiedFilename);
			return; 
		default: // should never get here as button should be disabled
			return;
	}	
}

void MainWindow::importSVGActionTriggered()
{
	QString userSpecifiedFilename = QFileDialog::getOpenFileName(this, 
		"Open file...", QDir::currentPath(), "Scalable vector graphics file (*.svg)");
	if (userSpecifiedFilename.isNull())
		return;

	SVG::SVG svg;
	Cane *newEditorCane = new Cane(PullTemplate::BASE_SQUARE);
	if (!SVG::load_svg(userSpecifiedFilename.toUtf8().constData(), svg, newEditorCane)) 
	{
		QMessageBox::warning(this, "Import failed", "Failed to read " + userSpecifiedFilename);
		deep_delete(newEditorCane);
		return;
	}

	if (svg.page.c[0] != svg.page.c[1]) 
	{
		QMessageBox::warning(this, "Import failed", 
			"The image in " + userSpecifiedFilename + " is not square.");
		deep_delete(newEditorCane);
		return;
	}

	caneLibraryLayout->addWidget(new CaneLibraryWidget(newEditorCane, this));
	setViewMode(PULLPLAN_VIEW_MODE);
	caneEditorWidget->setCane(newEditorCane);
}

void MainWindow::getDependantLibraryContents(Piece* piece, vector<GlassColor*>& colors, vector<Cane*>& canes, 
	vector<Piece*>& pieces)
{
	for (int i = 0; i < glassColorLibraryLayout->count(); ++i)
	{
		GlassColorLibraryWidget *cblw = dynamic_cast<GlassColorLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(glassColorLibraryLayout->itemAt(i))->widget());
		if (piece->hasDependencyOn(cblw->glassColor))
		{
			colors.push_back(cblw->glassColor);
			canes.push_back(cblw->circleCane);
			canes.push_back(cblw->squareCane);
		}
	}

	for (int i = 0; i < caneLibraryLayout->count(); ++i)
	{
		Cane* cane = dynamic_cast<CaneLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(caneLibraryLayout->itemAt(i))->widget())->cane;
		if (piece->hasDependencyOn(cane))
			canes.push_back(cane);
	}
	
	pieces.push_back(piece);	
}

void MainWindow::getDependantLibraryContents(Cane* cane, vector<GlassColor*>& colors, vector<Cane*>& canes)
{
	for (int i = 0; i < glassColorLibraryLayout->count(); ++i)
	{
		GlassColorLibraryWidget *cblw = dynamic_cast<GlassColorLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(glassColorLibraryLayout->itemAt(i))->widget());
		if (cane->hasDependencyOn(cblw->glassColor))
		{
			colors.push_back(cblw->glassColor);
			canes.push_back(cblw->circleCane);
			canes.push_back(cblw->squareCane);
		}
	}

	for (int i = 0; i < caneLibraryLayout->count(); ++i)
	{
		Cane* otherCane = dynamic_cast<CaneLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(caneLibraryLayout->itemAt(i))->widget())->cane;
		if (cane->hasDependencyOn(otherCane))
			canes.push_back(otherCane);
	}
}

void MainWindow::getDependantLibraryContents(GlassColor* color, vector<GlassColor*>& colors, vector<Cane*>& canes)
{
	for (int i = 0; i < glassColorLibraryLayout->count(); ++i)
	{
		GlassColorLibraryWidget *cblw = dynamic_cast<GlassColorLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(glassColorLibraryLayout->itemAt(i))->widget());
		if (cblw->glassColor == color)
		{
			colors.push_back(cblw->glassColor);
			canes.push_back(cblw->circleCane);
			canes.push_back(cblw->squareCane);
			break;
		}
	}
}

void MainWindow::getLibraryContents(vector<GlassColor*>& colors, vector<Cane*>& canes, vector<Piece*>& pieces)
{
	for (int i = 0; i < glassColorLibraryLayout->count(); ++i)
	{
		GlassColorLibraryWidget *cblw = dynamic_cast<GlassColorLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(glassColorLibraryLayout->itemAt(i))->widget());
		colors.push_back(cblw->glassColor);
		canes.push_back(cblw->circleCane);
		canes.push_back(cblw->squareCane);
	}

	for (int i = 0; i < caneLibraryLayout->count(); ++i)
	{
		CaneLibraryWidget* pplw = dynamic_cast<CaneLibraryWidget*>(
			dynamic_cast<QWidgetItem*>(caneLibraryLayout->itemAt(i))->widget());
		canes.push_back(pplw->cane);
	}

	for (int i = 0; i < pieceLibraryLayout->count(); ++i)
	{
		PieceLibraryWidget* plw = dynamic_cast<PieceLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
		pieces.push_back(plw->piece);
	}
}


void MainWindow::newFileActionTriggered()
{
	// ask the person what they want to do with the current state
	if (dirtyBit)
	{
		QMessageBox msgBox;
		msgBox.setText("The glass library has been modified.");
		msgBox.setInformativeText("Do you want to save your changes?");
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);

		int returnValue = msgBox.exec();

		switch (returnValue)
		{
			case QMessageBox::Cancel:
				return;
			case QMessageBox::Save:
				saveAllFileActionTriggered();
				break;
			case QMessageBox::Discard:
				break;
		}
	}

	// reset the system:

	// 1. set editor objects to new (but default) objects
	glassColorEditorWidget->resetGlassColor();
	caneEditorWidget->resetCane();
	pieceEditorWidget->resetPiece();

	// 2. delete everything in the library
	clearLibrary();

	// 3. add the three new guys from the editors into the library 
	glassColorLibraryLayout->addWidget(
		new GlassColorLibraryWidget(glassColorEditorWidget->glassColor(), this));
	caneLibraryLayout->addWidget(
		new CaneLibraryWidget(caneEditorWidget->cane(), this));
	pieceLibraryLayout->addWidget(
		new PieceLibraryWidget(pieceEditorWidget->piece(), this)); 

	// 4. go back to empty view mode
	setViewMode(EMPTY_VIEW_MODE);

	// reset filename
	setSaveFilename("[unsaved]");
	setDirtyBit(false);
}

void MainWindow::addToLibrary(vector<GlassColor*>& colors, vector<Cane*>& canes, vector<Piece*>& pieces)
{
	for (unsigned int i = 0; i < colors.size(); ++i)
	{
		// search ad hoc for canes that match the description of being 
		// the type of canes that live in color bar library widgets, and 
		// have the right color
		// 
		// this approach is biased *towards* sucking canes into color bar
		// library widgets. if you actually make canes that look exactly
		// like color bars (no subcanes, one casing), it will rip the first one
		// out of the cane list (where you had it) and put it in the 
		// corresponding color library widget. 
		//
		// preserving the library exactly in all cases would require explicitly 
		// writing membership in the save file, either in colors or canes.
		// we don't do this for now to keep the save files as simple as possible,
		// and bias the user's mental model towards "color bars live in color
		// objects" and away from "I need to make a new color bar cane to
		// use a color". 
		//
		// if the json file is never hand edited and the canes have the same label
		// ordering as that induced when writing to a file, then save/load preserves
		// the library exactly
		Cane* circleCane = NULL;
		for (unsigned int j = 0; j < canes.size(); ++j)
		{
			// we take the *first* matching cane because in getLibraryContents()
			// we add the color library widget canes to the canes list *first*.
			if (canes[j]->templateType() == PullTemplate::BASE_CIRCLE 
				&& canes[j]->casingCount() == 1
				&& canes[j]->outermostCasingColor() == colors[i])
			{
				circleCane = canes[j];
				canes.erase(canes.begin() + j);
				break;
			}
		}
		Cane* squareCane = NULL;
		for (unsigned int j = 0; j < canes.size(); ++j)
		{
			if (canes[j]->templateType() == PullTemplate::BASE_SQUARE 
				&& canes[j]->casingCount() == 1
				&& canes[j]->outermostCasingColor() == colors[i])
			{
				squareCane = canes[j];
				canes.erase(canes.begin() + j);
				break;
			}
		}
		glassColorLibraryLayout->addWidget(new GlassColorLibraryWidget(colors[i], this, 
			circleCane, squareCane));
	}
	for (unsigned int i = 0; i < canes.size(); ++i)
		caneLibraryLayout->addWidget(new CaneLibraryWidget(canes[i], this));
	for (unsigned int i = 0; i < pieces.size(); ++i)
		pieceLibraryLayout->addWidget(new PieceLibraryWidget(pieces[i], this));

}

void MainWindow::openFile(QString filename, bool add)
{
	// try to read in the file
	vector<GlassColor*> colors;
	vector<Cane*> canes;
	vector<Piece*> pieces;
	bool success = readGlassFile(filename, colors, canes, pieces);

	// if it failed, pop a sad little message box 
	if (!success) 
	{
		QMessageBox::warning(this, "Open failed", "The file " + filename + " cannot be read.");
		return;
	}		

	// deal with save filenames and dirty bits
	if (add)
	{
		setDirtyBit(true);
	}
	else 
	{
		setViewMode(EMPTY_VIEW_MODE);
		clearLibrary();	
		setSaveFilename(filename);
		setDirtyBit(false);
	}
	
	addToLibrary(colors, canes, pieces);	
}

void MainWindow::openFileActionTriggered()
{
	// first, if user's current file is dirty, confirm that they are ok to throw away their current changes
	// notice that if the open file dialog fails, then their changes won't actually be thrown away.
	if (dirtyBit)
	{
		QMessageBox msgBox;
		msgBox.setText("The glass library has been modified.");
		msgBox.setInformativeText("Do you want to discard your changes?");
		msgBox.setStandardButtons(QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		int returnValue = msgBox.exec();

		switch (returnValue)
		{
			case QMessageBox::Cancel:
				return;
			case QMessageBox::Discard:
				break;
		}
	}
	
	QString userSpecifiedFilename = QFileDialog::getOpenFileName(this, 
		"Open file...", QDir::currentPath(), "VirtualGlass glass file (*.glass)");
	if (userSpecifiedFilename.isNull())
		return;

	openFile(userSpecifiedFilename, false);
}

void MainWindow::addFileActionTriggered()
{
	QStringList userSpecifiedFilenames = QFileDialog::getOpenFileNames(this, 
		"Open file...", QDir::currentPath(), "VirtualGlass glass file (*.glass)");
	if (userSpecifiedFilenames.size() == 0) // emptiness iff empty list (hopefully?)
		return;

	// try to read in the files....ALL OF THEM
	for (int i = 0; i < userSpecifiedFilenames.size(); ++i)
		openFile(userSpecifiedFilenames[i], true);
}

void MainWindow :: emailSuccess(QString to)
{
	showStatusMessage("Email sent successfully.", 3);	
	QMessageBox::information(this, "Email success", "Message successfully sent to " + to + ".");
	switch (editorStack->currentIndex())
	{
		case PULLPLAN_VIEW_MODE:
		case PIECE_VIEW_MODE:
			shareFileButton->setEnabled(!email->sending());
			break;
		default:
			shareFileButton->setEnabled(false);
	}
}

void MainWindow :: emailFailure(QString error)
{
	showStatusMessage("Email failed.", 3);	
	QMessageBox::warning(this, "Email failed", "Failed to send message: " + error + ".");
	switch (editorStack->currentIndex())
	{
		case PULLPLAN_VIEW_MODE:
		case PIECE_VIEW_MODE:
			shareFileButton->setEnabled(!email->sending());
			break;
		default:
			shareFileButton->setEnabled(false);
	}
}

void MainWindow::shareFileActionTriggered()
{
	switch (editorStack->currentIndex())
	{
		case EMPTY_VIEW_MODE:
		case COLORBAR_VIEW_MODE:
			return;
	}

	bool ok;
	QString userSpecifiedAddress = QInputDialog::getText(this, "Email your design", "Email address:", 
		QLineEdit::Normal, "friend@internet.com", &ok);
	
	// Check for basic validity
	if (!ok || userSpecifiedAddress.isEmpty()) 
		return;

	// Check for email address validity?
	if (!userSpecifiedAddress.contains("@"))
	{
		QMessageBox::warning(this, "Invalid email address", userSpecifiedAddress + " is an invalid email address.");
		return;	
	}

	
	// Step 1. Grab selected object and dependancies and write them to a temp file
	vector<GlassColor*> colors;
	vector<Cane*> canes;
	vector<Piece*> pieces;
	switch (editorStack->currentIndex())
	{
		case PULLPLAN_VIEW_MODE:
			getDependantLibraryContents(caneEditorWidget->cane(), colors, canes);
			break;
		case PIECE_VIEW_MODE:
			getDependantLibraryContents(pieceEditorWidget->piece(), colors, canes, pieces);
			break;
	}
	QBuffer glassFileBuffer;
	writeGlassFile(glassFileBuffer, colors, canes, pieces);	


	// Step 2. Grab a screenshot and write it to the buffer
	QImage screenshot;
	switch (editorStack->currentIndex())
	{
		case PULLPLAN_VIEW_MODE:
			screenshot = caneEditorWidget->caneImage();
			break;
		case PIECE_VIEW_MODE:
			screenshot = pieceEditorWidget->pieceImage();
			break;
	}
	QBuffer screenshotFileBuffer;
	QImageWriter screenshotWriter(&screenshotFileBuffer, "png");
	screenshotWriter.write(screenshot);


	// Step 3. Try to send the email
	shareFileButton->setEnabled(false);	
	showStatusMessage("Sending email...", 3);	
	email->send(userSpecifiedAddress, QString("shared design"), glassFileBuffer, screenshotFileBuffer, "png");
}

void MainWindow::saveAllFileActionTriggered()
{
	if (saveFilename == "[unsaved]")
		saveAllAsFileActionTriggered();
	else
	{
		// call the actual file-saving code in GlassFileWriter
		vector<GlassColor*> colors;
		vector<Cane*> colorCanes;
		vector<Cane*> canes;
		vector<Piece*> pieces;
		getLibraryContents(colors, canes, pieces);
		writeGlassFile(saveFilename, colors, canes, pieces);	
		setDirtyBit(false);
	}
}

void MainWindow::saveAllAsFileActionTriggered()
{
	// get a filename from the user, suggesting "untitled.glass".
	QString userSpecifiedFilename = QFileDialog::getSaveFileName(this, "Save as...", 
		"untitled.glass", "VirtualGlass glass file (*.glass)");

	// check that the user chose something
	if (userSpecifiedFilename.isNull())
		return;
	// if they didn't chose a .glass file, make it one
	if (!userSpecifiedFilename.endsWith(".glass"))
		userSpecifiedFilename += ".glass";
	// set the program's running savefile to this name
	setSaveFilename(userSpecifiedFilename);
	
	vector<GlassColor*> colors;
	vector<Cane*> canes;
	vector<Piece*> pieces;
	getLibraryContents(colors, canes, pieces);
	writeGlassFile(saveFilename, colors, canes, pieces);	
	setDirtyBit(false);
}

void MainWindow::saveSelectedAsFileActionTriggered()
{
	if (editorStack->currentIndex() == EMPTY_VIEW_MODE)
		return;

	// get a filename from the user, suggesting "untitled.glass".
	QString userSpecifiedFilename = QFileDialog::getSaveFileName(this, "Save as...", 
		"untitled.glass", "VirtualGlass glass file (*.glass)");

	// check that the user chose something
	if (userSpecifiedFilename.isNull())
		return;
	// if they didn't chose a .glass file, make it one
	if (!userSpecifiedFilename.endsWith(".glass"))
		userSpecifiedFilename += ".glass";

	vector<GlassColor*> colors;
	vector<Cane*> canes;
	vector<Piece*> pieces;
	switch (editorStack->currentIndex())
	{
		case COLORBAR_VIEW_MODE:
			getDependantLibraryContents(glassColorEditorWidget->glassColor(), colors, canes);
			break;
		case PULLPLAN_VIEW_MODE:
			getDependantLibraryContents(caneEditorWidget->cane(), colors, canes);
			break;
		case PIECE_VIEW_MODE:
			getDependantLibraryContents(pieceEditorWidget->piece(), colors, canes, pieces);
			break;
	}
	
	// pass off "curated" library to regular write file; 
	// effectively pretending that the library only has your thing and its dependancies
	writeGlassFile(userSpecifiedFilename, colors, canes, pieces);	

	// this doesn't impact dirty bit or saveFilename at all: it's a special operation that 
	// virtualglass has that lives outside of the usual file-editor relationship, e.g. of a text editor. 
}




