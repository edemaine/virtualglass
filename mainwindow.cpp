
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QStackedWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QPushButton>
#include <QMouseEvent>
#include <QDrag>
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

#include "constants.h"
#include "dependancy.h"
#include "email.h"
#include "niceviewwidget.h"
#include "piecelibrarywidget.h"
#include "pullplanlibrarywidget.h"
#include "glasscolorlibrarywidget.h"
#include "pullplan.h"
#include "pulltemplate.h"
#include "pickupplan.h"
#include "piece.h"
#include "pullplaneditorwidget.h"
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
	setupToolbar();
	setupLibrary();
	setupEditors();
	setupMenus();
	setupSaveFile();
	setupStatusBar();
	setupConnections();
	setWindowTitle(windowTitle());

	// do the finishing touches to put the GUI in fresh state
	setViewMode(EMPTY_VIEW_MODE);
	show();
	setDirtyBit(false);
}


void MainWindow :: setViewMode(enum ViewMode _mode)
{
	editorStack->setCurrentIndex(_mode);
	switch (_mode)
	{
		case EMPTY_VIEW_MODE:
			newObjectButton->setEnabled(false);
			copyObjectButton->setEnabled(false);
			deleteObjectButton->setEnabled(false);
			exportPLYFileAction->setEnabled(false);
			exportOBJFileAction->setEnabled(false);
			saveSelectedAsFileAction->setEnabled(false);
			break;
		case COLORBAR_VIEW_MODE:
			newObjectButton->setEnabled(true);
			copyObjectButton->setEnabled(true);
			deleteObjectButton->setEnabled(true);
			exportPLYFileAction->setEnabled(false);
			exportOBJFileAction->setEnabled(false);
			saveSelectedAsFileAction->setEnabled(true);
			break;
		case PULLPLAN_VIEW_MODE:
			newObjectButton->setEnabled(true);
			copyObjectButton->setEnabled(true);
			deleteObjectButton->setEnabled(true);
			exportPLYFileAction->setEnabled(true);
			exportOBJFileAction->setEnabled(true);
			saveSelectedAsFileAction->setEnabled(true);
			break;
		case PIECE_VIEW_MODE:
			newObjectButton->setEnabled(true);
			copyObjectButton->setEnabled(true);
			deleteObjectButton->setEnabled(true);
			exportPLYFileAction->setEnabled(true);
			exportOBJFileAction->setEnabled(true);
			saveSelectedAsFileAction->setEnabled(true);
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

void MainWindow :: unhighlightAllLibraryWidgets()
{
	QLayoutItem* w;
	for (int j = 0; j < colorBarLibraryLayout->count(); ++j)
	{
		w = colorBarLibraryLayout->itemAt(j);
		static_cast<GlassColorLibraryWidget*>(w->widget())->setDependancy(false);
	}
	for (int j = 0; j < pullPlanLibraryLayout->count(); ++j)
	{
		w = pullPlanLibraryLayout->itemAt(j);
		static_cast<PullPlanLibraryWidget*>(w->widget())->setDependancy(false);
	}
	for (int j = 0; j < pieceLibraryLayout->count(); ++j)
	{
		w = pieceLibraryLayout->itemAt(j);
		static_cast<PieceLibraryWidget*>(w->widget())->setDependancy(false);
	}
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
			for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
			{
				QLayoutItem* w = colorBarLibraryLayout->itemAt(i);
				GlassColor* gc = dynamic_cast<GlassColorLibraryWidget*>(w->widget())->glassColor;
				if (gc == colorEditorWidget->getGlassColor())
				{
					w = colorBarLibraryLayout->takeAt(i);
					colorBarLibraryLayout->insertWidget(
						MIN(MAX(i+d, 0), colorBarLibraryLayout->count()), w->widget()); 
					return;
				}
			}	
			return;	
		case PULLPLAN_VIEW_MODE:
			for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				QLayoutItem* w = pullPlanLibraryLayout->itemAt(i);
				PullPlan* p = dynamic_cast<PullPlanLibraryWidget*>(w->widget())->pullPlan;
				if (p == pullPlanEditorWidget->getPullPlan())
				{
					w = pullPlanLibraryLayout->takeAt(i);
					pullPlanLibraryLayout->insertWidget(
						MIN(MAX(i+d, 0), pullPlanLibraryLayout->count()), w->widget()); 
					return;
				}
			}
			return;	
		case PIECE_VIEW_MODE:
			for (int i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				QLayoutItem* w = pieceLibraryLayout->itemAt(i);
				Piece* p = dynamic_cast<PieceLibraryWidget*>(w->widget())->piece;
				if (p == pieceEditorWidget->getPiece())
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
	switch (editorStack->currentIndex())
	{
		case COLORBAR_VIEW_MODE:
		{
			if (colorBarLibraryLayout->count() == 1)
				return;

			int i;
			for (i = 0; i < colorBarLibraryLayout->count(); ++i)
			{
				QLayoutItem* w = colorBarLibraryLayout->itemAt(i);
				GlassColor* gc = dynamic_cast<GlassColorLibraryWidget*>(w->widget())->glassColor;
				if (gc == colorEditorWidget->getGlassColor())
				{
					if (glassColorIsDependancy(gc))
					{
						QMessageBox::warning(this, "Delete failed", 
							"This color cannot be deleted:\nother objects use it.");
					}
					else
					{
						// this may be a memory leak, the library widget is never explicitly deleted
						w = colorBarLibraryLayout->takeAt(i);
						delete w->widget();
						delete w;
					}
					break;
				}
			}

			colorEditorWidget->setGlassColor(dynamic_cast<GlassColorLibraryWidget*>(colorBarLibraryLayout->itemAt(
					MIN(colorBarLibraryLayout->count()-1, i))->widget())->glassColor);
			updateLibrary();
			setDirtyBit(true);
			break;
		}
		case PULLPLAN_VIEW_MODE:
		{
			if (pullPlanLibraryLayout->count() == 1)
				return;

			int i;
			for (i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				QLayoutItem* w = pullPlanLibraryLayout->itemAt(i);
				PullPlan* p = dynamic_cast<PullPlanLibraryWidget*>(w->widget())->pullPlan;
				if (p == pullPlanEditorWidget->getPullPlan())
				{
					if (pullPlanIsDependancy(p))
					{
						QMessageBox::warning(this, "Delete failed", 
							"This cane cannot be deleted:\nother objects use it.");
					}
					else
					{
						// this may be a memory leak, the library widget is never explicitly deleted
						w = pullPlanLibraryLayout->takeAt(i);
						delete w->widget();
						delete w;
					}
					break;
				}
			}

			pullPlanEditorWidget->setPullPlan(dynamic_cast<PullPlanLibraryWidget*>(pullPlanLibraryLayout->itemAt(
					MIN(pullPlanLibraryLayout->count()-1, i))->widget())->pullPlan);
			updateLibrary();
			setDirtyBit(true);
			break;
		}
		case PIECE_VIEW_MODE:
		{
			if (pieceLibraryLayout->count() == 1)
				return;

			int i;
			for (i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				QLayoutItem* w = pieceLibraryLayout->itemAt(i);
				Piece* p = dynamic_cast<PieceLibraryWidget*>(w->widget())->piece;
				if (p == pieceEditorWidget->getPiece())
				{
					// this may be a memory leak, the library widget is never explicitly deleted
					w = pieceLibraryLayout->takeAt(i);
					delete w->widget();
					delete w;
					break;
				}
			}

			pieceEditorWidget->setPiece(dynamic_cast<PieceLibraryWidget*>(pieceLibraryLayout->itemAt(
					MIN(pieceLibraryLayout->count()-1, i))->widget())->piece);
			updateLibrary();
			setDirtyBit(true);
			break;
		}
	}

}

void MainWindow :: mousePressEvent(QMouseEvent* event)
{
	// bounding box for the geometry of the libraryScrollArea relative to libraryMasterWidget
	QRect scrollAreaRect = libraryScrollArea->geometry();
	// bounding box for the geometry of the libraryScrollArea relative to MainWindow
	scrollAreaRect.moveTopLeft(libraryScrollArea->parentWidget()->pos());
	
	if (event->button() == Qt::LeftButton && scrollAreaRect.contains(event->pos()))
	{
		isDragging = true;
		lastDragPosition = dragStartPosition = event->pos();
		maxDragDistance = 0;
	}
	else
		isDragging = false;
}

void MainWindow :: mouseMoveEvent(QMouseEvent* event)
{
	// If the left mouse button isn't down
	if ((event->buttons() & Qt::LeftButton) == 0)
	{
		isDragging = false;
		return;
	}

	int totalXMovement = fabs(event->pos().x() - dragStartPosition.x());
	int totalYMovement = fabs(event->pos().y() - dragStartPosition.y());
	if (!isDragging || MAX(totalXMovement, totalYMovement) < QApplication::startDragDistance())
		return;

	// If this is the first time you've exceeded the threshold for movement
	// and the movement starts a drag
	if (maxDragDistance < QApplication::startDragDistance() && totalXMovement >= totalYMovement)
	{
		GlassColorLibraryWidget* cblw = dynamic_cast<GlassColorLibraryWidget*>(childAt(event->pos()));
		PullPlanLibraryWidget* plplw = dynamic_cast<PullPlanLibraryWidget*>(childAt(event->pos()));

		char buf[500];
		QPixmap pixmap;

		if (cblw != NULL)
		{
			GlassMime::encode(buf, cblw, GlassMime::COLORLIBRARY_MIME);
			pixmap = *cblw->getDragPixmap();
		}
		else if (plplw != NULL)
		{
			GlassMime::encode(buf, plplw->pullPlan, GlassMime::PULLPLAN_MIME);
			pixmap = *plplw->getDragPixmap();
		}
		else
			return;

		QByteArray pointerData(buf);
		QMimeData* mimeData = new QMimeData;
		mimeData->setText(pointerData);

		QDrag *drag = new QDrag(this);
		drag->setMimeData(mimeData);
		drag->setPixmap(pixmap);
		drag->setHotSpot(QPoint(50, 100));

		drag->exec(Qt::CopyAction);

		// no longer a drag in the library (becomes a QDrag, i.e. a drag in program)	
		isDragging = false; 
	}
	// Otherwise  
	else 
	{
		int movement = event->pos().y() - lastDragPosition.y();
		libraryScrollArea->verticalScrollBar()->setValue(
			libraryScrollArea->verticalScrollBar()->value() - movement);
		lastDragPosition = event->pos();

		maxDragDistance = MAX(maxDragDistance, totalYMovement);
	}
}

void MainWindow :: mouseReleaseEvent(QMouseEvent* event)
{
	// If this is a drop and not the end of a click, don't process (dropEvent() will do it instead)
	if (!isDragging || (isDragging && maxDragDistance >= QApplication::startDragDistance()))
		return;

	GlassColorLibraryWidget* cblw = dynamic_cast<GlassColorLibraryWidget*>(childAt(event->pos()));
	PullPlanLibraryWidget* plplw = dynamic_cast<PullPlanLibraryWidget*>(childAt(event->pos()));
	PieceLibraryWidget* plw = dynamic_cast<PieceLibraryWidget*>(childAt(event->pos()));

	if (cblw != NULL)
	{
		setViewMode(COLORBAR_VIEW_MODE);
		colorEditorWidget->setGlassColor(cblw->glassColor);
	}
	else if (plplw != NULL)
	{
		setViewMode(PULLPLAN_VIEW_MODE);
		pullPlanEditorWidget->setPullPlan(plplw->pullPlan);
	}
	else if (plw != NULL)
	{
		setViewMode(PIECE_VIEW_MODE);
		pieceEditorWidget->setPiece(plw->piece);
	}

	updateLibrary();
}


void MainWindow :: setupConnections()
{
	connect(colorEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateLibrary()));
	connect(colorEditorWidget, SIGNAL(someDataChanged()), this, SLOT(setDirtyBitTrue()));

	connect(pullPlanEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateLibrary()));
	connect(pullPlanEditorWidget, SIGNAL(someDataChanged()), this, SLOT(setDirtyBitTrue()));

	connect(pieceEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateLibrary()));
	connect(pieceEditorWidget, SIGNAL(someDataChanged()), this, SLOT(setDirtyBitTrue()));

	connect(newFileButton, SIGNAL(clicked()), this, SLOT(newFileActionTriggered()));
	connect(openFileButton, SIGNAL(clicked()), this, SLOT(openFileActionTriggered()));
	connect(saveFileButton, SIGNAL(clicked()), this, SLOT(saveAllFileActionTriggered()));
	connect(shareFileButton, SIGNAL(clicked()), this, SLOT(shareFileActionTriggered()));
	connect(exampleCaneButton, SIGNAL(clicked()), this, SLOT(randomComplexCaneExampleActionTriggered()));
	connect(examplePieceButton, SIGNAL(clicked()), this, SLOT(randomComplexPieceExampleActionTriggered()));

	connect(newObjectButton, SIGNAL(clicked()), this, SLOT(newObject()));
	connect(copyObjectButton, SIGNAL(clicked()), this, SLOT(copyObject()));
	connect(deleteObjectButton, SIGNAL(clicked()), this, SLOT(deleteObject()));

	// the file menu stuff
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

	// the examples menu stuff
	connect(randomSimpleCaneAction, SIGNAL(triggered()), this, SLOT(randomSimpleCaneExampleActionTriggered()));
	connect(randomSimplePieceAction, SIGNAL(triggered()), this, SLOT(randomSimplePieceExampleActionTriggered()));

	connect(randomComplexCaneAction, SIGNAL(triggered()), this, SLOT(randomComplexCaneExampleActionTriggered()));
	connect(randomComplexPieceAction, SIGNAL(triggered()), this, SLOT(randomComplexPieceExampleActionTriggered()));

	// the performance menu stuff
	connect(depthPeelAction, SIGNAL(triggered()), this, SLOT(depthPeelActionTriggered()));

	// status bar stuff
	connect(pullPlanEditorWidget, SIGNAL(showMessage(const QString&, unsigned int)), 
		this, SLOT(showStatusMessage(const QString&, unsigned int)));
	connect(pieceEditorWidget, SIGNAL(showMessage(const QString&, unsigned int)), 
		this, SLOT(showStatusMessage(const QString&, unsigned int)));
}

void MainWindow :: depthPeelActionTriggered()
{
	GlobalDepthPeelingSetting::setEnabled(!GlobalDepthPeelingSetting::enabled());
	depthPeelAction->setChecked(GlobalDepthPeelingSetting::enabled());

	// lazily redraw only the current editor's view(s)
	switch (editorStack->currentIndex())
	{
		case COLORBAR_VIEW_MODE:
			colorEditorWidget->updateEverything();
			break;
		case PULLPLAN_VIEW_MODE:
			pullPlanEditorWidget->updateEverything();
			break;
		case PIECE_VIEW_MODE:
			pieceEditorWidget->updateEverything();
			break;
	}

	// redraw all library thumbnails
	for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
	{
		dynamic_cast<GlassColorLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget())->updatePixmaps();
	}
	for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
	{
		dynamic_cast<PullPlanLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget())->updatePixmaps();
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
	PullPlan* randomPP = randomSimplePullPlan(CIRCLE_SHAPE, randomGC);

	colorBarLibraryLayout->addWidget(new GlassColorLibraryWidget(randomGC, this));
	setDirtyBit(true);

	PullPlanLibraryWidget* pplw = new PullPlanLibraryWidget(randomPP, this);
	pullPlanLibraryLayout->addWidget(pplw);

	setViewMode(PULLPLAN_VIEW_MODE);
	pullPlanEditorWidget->setPullPlan(randomPP);
	updateLibrary();
}

void MainWindow :: randomComplexCaneExampleActionTriggered()
{
	GlassColor* randomGC = randomGlassColor();
	PullPlan* randomCPP = randomSimplePullPlan(CIRCLE_SHAPE, randomGC);
	PullPlan* randomSPP = randomSimplePullPlan(SQUARE_SHAPE, randomGC);
	PullPlan* randomComplexPP = randomComplexPullPlan(randomCPP, randomSPP);

	colorBarLibraryLayout->addWidget(new GlassColorLibraryWidget(randomGC, this));
	pullPlanLibraryLayout->addWidget(new PullPlanLibraryWidget(randomComplexPP, this));
	setDirtyBit(true);

	// add simple plans only if they are used
	// memory leak! as unused ones never appear in library
	if (randomComplexPP->hasDependencyOn(randomCPP))
		pullPlanLibraryLayout->addWidget(new PullPlanLibraryWidget(randomCPP, this));
	if (randomComplexPP->hasDependencyOn(randomSPP))
		pullPlanLibraryLayout->addWidget(new PullPlanLibraryWidget(randomSPP, this));

	setViewMode(PULLPLAN_VIEW_MODE);
	pullPlanEditorWidget->setPullPlan(randomComplexPP);
	updateLibrary();
}

void MainWindow :: randomSimplePieceExampleActionTriggered()
{
	GlassColor* randomGC = randomGlassColor();
	PullPlan* randomSPP = randomSimplePullPlan(SQUARE_SHAPE, randomGC);
	Piece* randomP = randomPiece(randomPickup(randomSPP));

	colorBarLibraryLayout->addWidget(new GlassColorLibraryWidget(randomGC, this));
	pullPlanLibraryLayout->addWidget(new PullPlanLibraryWidget(randomSPP, this));
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
	PullPlan* randomCPP = randomSimplePullPlan(CIRCLE_SHAPE, randomGC1);
	PullPlan* randomSPP = randomSimplePullPlan(SQUARE_SHAPE, randomGC2);
	PullPlan* randomComplexPP1 = randomComplexPullPlan(randomCPP, randomSPP);
	PullPlan* randomComplexPP2 = randomComplexPullPlan(randomCPP, randomSPP);
	Piece* randomP = randomPiece(randomPickup(randomComplexPP1, randomComplexPP2));

	if (randomP->hasDependencyOn(randomGC1)) // memory leak if returns no
		colorBarLibraryLayout->addWidget(new GlassColorLibraryWidget(randomGC1, this));
	if (randomP->hasDependencyOn(randomGC2)) // memory leak if returns no
		colorBarLibraryLayout->addWidget(new GlassColorLibraryWidget(randomGC2, this));
	if (randomP->hasDependencyOn(randomCPP)) // memory leak if returns no
		pullPlanLibraryLayout->addWidget(new PullPlanLibraryWidget(randomCPP, this));
	if (randomP->hasDependencyOn(randomSPP)) // memory leak if returns no
		pullPlanLibraryLayout->addWidget(new PullPlanLibraryWidget(randomSPP, this));
	pullPlanLibraryLayout->addWidget(new PullPlanLibraryWidget(randomComplexPP1, this));
	pullPlanLibraryLayout->addWidget(new PullPlanLibraryWidget(randomComplexPP2, this));
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

void MainWindow :: setupToolbar()
{
	QWidget* toolbarMasterWidget = new QWidget(centralWidget);
	centralLayout->addWidget(toolbarMasterWidget);

	QVBoxLayout* toolbarLayout = new QVBoxLayout(toolbarMasterWidget);
	toolbarMasterWidget->setLayout(toolbarLayout);
	toolbarLayout->setContentsMargins(0, 0, 0, 0);

	const int icon_size = 70;

	newFileButton = new QToolButton(toolbarMasterWidget);
	newFileButton->setFixedSize(icon_size, icon_size);
	newFileButton->setText("New file");
	newFileButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	toolbarLayout->addWidget(newFileButton);

	openFileButton = new QToolButton(toolbarMasterWidget);
	openFileButton->setFixedSize(icon_size, icon_size);
	openFileButton->setText("Open file");
	toolbarLayout->addWidget(openFileButton);

	saveFileButton = new QToolButton(toolbarMasterWidget);
	saveFileButton->setFixedSize(icon_size, icon_size);
	saveFileButton->setText("Save");
	toolbarLayout->addWidget(saveFileButton);
	
	shareFileButton = new QToolButton(toolbarMasterWidget);
	shareFileButton->setFixedSize(icon_size, icon_size);
	shareFileButton->setText("Share");
	toolbarLayout->addWidget(shareFileButton);
	
	toolbarLayout->addSpacing(icon_size);

	exampleCaneButton = new QToolButton(toolbarMasterWidget);
	exampleCaneButton->setFixedSize(icon_size, icon_size);
	exampleCaneButton->setText("Random\ncane");
	toolbarLayout->addWidget(exampleCaneButton);

	examplePieceButton = new QToolButton(toolbarMasterWidget);
	examplePieceButton->setFixedSize(icon_size, icon_size);
	examplePieceButton->setText("Random\npiece");
	toolbarLayout->addWidget(examplePieceButton);

	toolbarLayout->addStretch(1);
}

void MainWindow :: setupLibrary()
{
	QWidget* libraryMasterWidget = new QWidget(centralWidget);
	centralLayout->addWidget(libraryMasterWidget);

	QVBoxLayout* libraryAreaLayout = new QVBoxLayout(libraryMasterWidget);
	libraryMasterWidget->setLayout(libraryAreaLayout);
	libraryAreaLayout->setContentsMargins(0, 0, 0, 0);

	libraryScrollArea = new QScrollArea(libraryMasterWidget);
	libraryAreaLayout->addWidget(libraryScrollArea, 1);
	// Filter out up/down arrow key events for moving library objects around.
	// See eventFilter() for details. 
	libraryScrollArea->installEventFilter(this);	
	libraryScrollArea->setBackgroundRole(QPalette::Dark);
	libraryScrollArea->setWidgetResizable(true);
	libraryScrollArea->setFixedWidth(370);
	libraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	libraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	QWidget* libraryWidget = new QWidget(libraryScrollArea);
	libraryScrollArea->setWidget(libraryWidget);

	// TODO: refactor library to only use one layout
	QGridLayout* superlibraryLayout = new QGridLayout(libraryWidget);
	libraryWidget->setLayout(superlibraryLayout);
	superlibraryLayout->setContentsMargins(0, 9, 0, 9);

	newObjectButton = new QPushButton("New", libraryWidget);
	superlibraryLayout->addWidget(newObjectButton, 0, 0);

	copyObjectButton = new QPushButton("Copy", libraryWidget);
	superlibraryLayout->addWidget(copyObjectButton, 0, 1);

	deleteObjectButton = new QPushButton("Delete", libraryWidget);
	superlibraryLayout->addWidget(deleteObjectButton, 0, 2);

	colorBarLibraryLayout = new QVBoxLayout(libraryWidget);
	colorBarLibraryLayout->setSpacing(10);
	colorBarLibraryLayout->setDirection(QBoxLayout::BottomToTop);
	pullPlanLibraryLayout = new QVBoxLayout(libraryWidget);
	pullPlanLibraryLayout->setSpacing(10);
	pullPlanLibraryLayout->setDirection(QBoxLayout::BottomToTop);
	pieceLibraryLayout = new QVBoxLayout(libraryWidget);
	pieceLibraryLayout->setSpacing(10);
	pieceLibraryLayout->setDirection(QBoxLayout::BottomToTop);
	superlibraryLayout->addLayout(colorBarLibraryLayout, 1, 0, Qt::AlignTop);
	superlibraryLayout->addLayout(pullPlanLibraryLayout, 1, 1, Qt::AlignTop);
	superlibraryLayout->addLayout(pieceLibraryLayout, 1, 2, Qt::AlignTop);

	// make three qlabels for a legend
	QWidget* legendWidget = new QWidget(libraryMasterWidget);
	libraryAreaLayout->addWidget(legendWidget);

	QGridLayout* legendLayout = new QGridLayout(legendWidget);
	legendWidget->setLayout(legendLayout);
	legendLayout->setColumnStretch(0, 1);
	legendLayout->setColumnStretch(1, 1);
	legendLayout->setColumnStretch(2, 1);
	legendLayout->setContentsMargins(9, 10, 9, 10); // hack to make legend height same as normal

	QLabel* l1 = new QLabel("Used By Selected", legendWidget);
	l1->setStyleSheet("border: 2px dashed " + QColor(0, 139, 69, 255).name() + ";");
	legendLayout->addWidget(l1, 0, 0, Qt::AlignCenter);
	QLabel* l2 = new QLabel("  Selected  ", legendWidget);
	l2->setStyleSheet("border: 2px solid " + QColor(0, 0, 255, 255).name() + ";");
	legendLayout->addWidget(l2, 0, 1, Qt::AlignCenter);
	QLabel* l3 = new QLabel("Uses Selected", legendWidget);
	l3->setStyleSheet("border: 2px dotted " + QColor(200, 100, 0, 255).name() + ";");
	legendLayout->addWidget(l3, 0, 2, Qt::AlignCenter);

	QLabel* descriptionLabel = new QLabel("Library - click to edit or drag to add", legendWidget);
	descriptionLabel->setAlignment(Qt::AlignCenter);
	libraryAreaLayout->addWidget(descriptionLabel);
}

void MainWindow :: clearLibrary()
{
	// kind of a memory leak here, as we delete
	// all of the library objects but none of the glass objects they represent
	// the non-trivial part is just the editors and their "current editing object"
	QLayoutItem* w;
	
	while (colorBarLibraryLayout->count() > 0)
	{
		w = colorBarLibraryLayout->takeAt(0);
		delete w->widget();
		delete w;
	}
	while (pullPlanLibraryLayout->count() > 0)
	{
		w = pullPlanLibraryLayout->takeAt(0);
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
	editorStack->addWidget(colorEditorWidget);

	setupPullPlanEditor();
	editorStack->addWidget(pullPlanEditorWidget);

	setupPieceEditor();
	editorStack->addWidget(pieceEditorWidget);
}

void MainWindow :: setupColorEditor()
{
	// Setup data objects - the current plan and library widget for this plan
	colorEditorWidget = new ColorEditorWidget(editorStack);
	colorBarLibraryLayout->addWidget(new GlassColorLibraryWidget(colorEditorWidget->getGlassColor(), this));
	colorEditorWidget->updateEverything();
}

void MainWindow :: setupPullPlanEditor()
{
	// Setup data objects - the current plan and library widget for this plan
	pullPlanEditorWidget = new PullPlanEditorWidget(editorStack);
	pullPlanLibraryLayout->addWidget(new PullPlanLibraryWidget(pullPlanEditorWidget->getPullPlan(), this));
	pullPlanEditorWidget->updateEverything();
}

void MainWindow :: setupPieceEditor()
{
	// Setup data objects - the current plan and library widget for this plan
	pieceEditorWidget = new PieceEditorWidget(editorStack);
	pieceLibraryLayout->addWidget(new PieceLibraryWidget(pieceEditorWidget->getPiece(), this));
	pieceEditorWidget->updateEverything();
}

void MainWindow :: newObject()
{
	switch (editorStack->currentIndex())
	{
		case COLORBAR_VIEW_MODE:
			newGlassColor();
			break;
		case PULLPLAN_VIEW_MODE:
			newPullPlan();
			break;
		case PIECE_VIEW_MODE:
			newPiece();
			break;
	}
}

void MainWindow :: copyObject()
{
	switch (editorStack->currentIndex())
	{
		case COLORBAR_VIEW_MODE:
			copyGlassColor();
			break;
		case PULLPLAN_VIEW_MODE:
			copyPullPlan();
			break;
		case PIECE_VIEW_MODE:
			copyPiece();
			break;
	}
}

void MainWindow :: deleteObject()
{
	switch (editorStack->currentIndex())
	{
		case COLORBAR_VIEW_MODE:
		case PULLPLAN_VIEW_MODE:
		case PIECE_VIEW_MODE:
			deleteCurrentEditingObject();
			break;
	}
}

void MainWindow :: newPiece()
{
	Piece* newEditorPiece = new Piece(PieceTemplate::TUMBLER);
	pieceLibraryLayout->addWidget(new PieceLibraryWidget(newEditorPiece, this));
	setViewMode(PIECE_VIEW_MODE);
	pieceEditorWidget->setPiece(newEditorPiece);
	updateLibrary();
}

void MainWindow :: copyPiece()
{
	Piece* newEditorPiece = pieceEditorWidget->getPiece()->copy();
	pieceLibraryLayout->addWidget(new PieceLibraryWidget(newEditorPiece, this));
	pieceEditorWidget->setPiece(newEditorPiece);
	updateLibrary();
}

void MainWindow :: newGlassColor()
{
	GlassColor* newGlassColor = new GlassColor();
	colorBarLibraryLayout->addWidget(new GlassColorLibraryWidget(newGlassColor, this));
	setViewMode(COLORBAR_VIEW_MODE);
	colorEditorWidget->setGlassColor(newGlassColor);
	updateLibrary();
}

void MainWindow :: copyGlassColor()
{
	GlassColor* newEditorGlassColor = colorEditorWidget->getGlassColor()->copy();
	colorBarLibraryLayout->addWidget(new GlassColorLibraryWidget(newEditorGlassColor, this));
	colorEditorWidget->setGlassColor(newEditorGlassColor);
	updateLibrary();
}

void MainWindow :: newPullPlan()
{
	PullPlan *newEditorPlan = new PullPlan(PullTemplate::HORIZONTAL_LINE_CIRCLE);
	pullPlanLibraryLayout->addWidget(new PullPlanLibraryWidget(newEditorPlan, this));
	setViewMode(PULLPLAN_VIEW_MODE);
	pullPlanEditorWidget->setPullPlan(newEditorPlan);
	updateLibrary();
}

void MainWindow :: copyPullPlan()
{
	PullPlan *newEditorPlan = pullPlanEditorWidget->getPullPlan()->copy();
	pullPlanLibraryLayout->addWidget(new PullPlanLibraryWidget(newEditorPlan, this));
	pullPlanEditorWidget->setPullPlan(newEditorPlan);
	updateLibrary();
}

// returns whether the pull plan is a dependancy of something in the library
// (either a pull plan or a piece)
bool MainWindow :: glassColorIsDependancy(GlassColor* color)
{
	PullPlanLibraryWidget* pplw;
	for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
	{
		pplw = dynamic_cast<PullPlanLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
		if (pplw->pullPlan->hasDependencyOn(color))
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

// returns whether the pull plan is a dependancy of something in the library
// (either another pull plan or a piece)
bool MainWindow :: pullPlanIsDependancy(PullPlan* plan)
{
	PullPlanLibraryWidget* pplw;
	for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
	{
		pplw = dynamic_cast<PullPlanLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
		// Check whether the pull plan in the library is:
		// 1. the parameter plan
		// 2. a plan with the plan currently being edited as a subplan
		if (pplw->pullPlan == plan)
		{
			continue;
		}
		else if (pplw->pullPlan->hasDependencyOn(plan))
		{
			return true;
		}
	}

	PieceLibraryWidget* plw;
	for (int i = 0; i < pieceLibraryLayout->count(); ++i)
	{
		plw = dynamic_cast<PieceLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
		if (plw->piece->hasDependencyOn(plan))
		{
			return true;
		}
	}

	return false;
}

void MainWindow :: updateLibrary()
{
	unhighlightAllLibraryWidgets();

	switch (editorStack->currentIndex())
	{
		case COLORBAR_VIEW_MODE:
		{
			for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
			{
				GlassColorLibraryWidget* cblw = dynamic_cast<GlassColorLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
				if (colorEditorWidget->getGlassColor() == cblw->glassColor)
				{
					cblw->updatePixmaps();
					cblw->setDependancy(true, IS_DEPENDANCY);
				}
			}

			for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				PullPlanLibraryWidget* pplw = dynamic_cast<PullPlanLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
				if (pplw->pullPlan->hasDependencyOn(colorEditorWidget->getGlassColor()))
				{
					pplw->updatePixmaps();
					pplw->setDependancy(true, USES_DEPENDANCY);
				}
			}

			for (int i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				PieceLibraryWidget* plw = dynamic_cast<PieceLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
				if (plw->piece->hasDependencyOn(colorEditorWidget->getGlassColor()))
				{
					plw->updatePixmap();
					plw->setDependancy(true, USES_DEPENDANCY);
				}
			}

			break;
		}
		case PULLPLAN_VIEW_MODE:
		{
			for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
			{
				GlassColorLibraryWidget* cblw = dynamic_cast<GlassColorLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
				if (pullPlanEditorWidget->getPullPlan()->hasDependencyOn(cblw->glassColor))
					cblw->setDependancy(true, USEDBY_DEPENDANCY);
			}
			for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				PullPlanLibraryWidget* pplw = dynamic_cast<PullPlanLibraryWidget*>(
						dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
				// Check whether the pull plan in the library is:
				// 1. the plan currently being edited
				// 2. a subplan of the plan current being edited
				// 3. a plan with the plan currently being edited as a subplan
				if (pullPlanEditorWidget->getPullPlan() == pplw->pullPlan)
				{
					pplw->updatePixmaps();
					pplw->setDependancy(true, IS_DEPENDANCY);
				}
				else if (pullPlanEditorWidget->getPullPlan()->hasDependencyOn(pplw->pullPlan))
				{
					pplw->setDependancy(true, USEDBY_DEPENDANCY);
				}
				else if (pplw->pullPlan->hasDependencyOn(pullPlanEditorWidget->getPullPlan()))
				{
					pplw->updatePixmaps();
					pplw->setDependancy(true, USES_DEPENDANCY);
				}
			}
			for (int i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				PieceLibraryWidget* plw = dynamic_cast<PieceLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
				if (plw->piece->hasDependencyOn(pullPlanEditorWidget->getPullPlan()))
				{
					plw->updatePixmap();
					plw->setDependancy(true, USES_DEPENDANCY);
				}
			}

			break;
		}
		case PIECE_VIEW_MODE:
		{
			for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
			{
				GlassColorLibraryWidget* cblw = dynamic_cast<GlassColorLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
				if (pieceEditorWidget->getPiece()->hasDependencyOn(cblw->glassColor))
					cblw->setDependancy(true, USEDBY_DEPENDANCY);
			}
			for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				PullPlanLibraryWidget* pplw = dynamic_cast<PullPlanLibraryWidget*>(
					dynamic_cast<QWidgetItem*>(pullPlanLibraryLayout->itemAt(i))->widget());
				if (pieceEditorWidget->getPiece()->hasDependencyOn(pplw->pullPlan))
					pplw->setDependancy(true, USEDBY_DEPENDANCY);
			}
			for (int i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				PieceLibraryWidget* plw = dynamic_cast<PieceLibraryWidget*>(
						dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
				if (plw->piece == pieceEditorWidget->getPiece())
				{
					plw->updatePixmap();
					plw->setDependancy(true, IS_DEPENDANCY);
				}
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
	//new
	newFileAction = new QAction("&New", this);
	newFileAction->setShortcuts(QKeySequence::New);
	newFileAction->setToolTip("Open a new empty file.");

	//open
	openFileAction = new QAction("&Open", this);
	openFileAction->setShortcuts(QKeySequence::Open);
	openFileAction->setToolTip("Open an existing file.");

	//add
	addFileAction = new QAction("&Add", this);
	addFileAction->setToolTip("Add an existing file.");

	//import svg cane
	importSVGFileAction = new QAction("&Import cane from .svg", this);
	importSVGFileAction->setToolTip("Import cane cross section from .svg file.");

	//export ply object
	exportPLYFileAction = new QAction("&Export glass to .ply", this);
	exportPLYFileAction->setToolTip("Export cane or piece");
	
	//export obj object
	exportOBJFileAction = new QAction("&Export glass to .obj", this);
	exportOBJFileAction->setToolTip("Export cane or piece");
	
	//save
	saveAllFileAction = new QAction("&Save", this);
	saveAllFileAction->setShortcuts(QKeySequence::Save);
	saveAllFileAction->setToolTip("Save library to file.");

	//saveAllAs
	saveAllAsFileAction = new QAction("Save &As", this);
	saveAllAsFileAction->setShortcuts(QKeySequence::SaveAs);
	saveAllAsFileAction->setToolTip("Save library to file.");

	//saveSelectedAs
	saveSelectedAsFileAction = new QAction("Save Selected As", this);
	saveSelectedAsFileAction->setToolTip("Save selected object to file.");

	//exit
	exitAction = new QAction(tr("Q&uit"), this);
	exitAction->setShortcuts(QKeySequence::Quit);
	exitAction->setToolTip("Quit");

	//File menu
	fileMenu = menuBar()->addMenu(tr("&File")); 
	fileMenu->addAction(newFileAction); 
	fileMenu->addAction(openFileAction); 
	fileMenu->addAction(addFileAction); 
	fileMenu->addSeparator();
	fileMenu->addAction(saveAllFileAction); 
	fileMenu->addAction(saveAllAsFileAction); 
	fileMenu->addAction(saveSelectedAsFileAction); 
	fileMenu->addSeparator();
	fileMenu->addAction(importSVGFileAction); 
	fileMenu->addAction(exportPLYFileAction); 
	fileMenu->addAction(exportOBJFileAction); 
	fileMenu->addSeparator();
	fileMenu->addAction(exitAction);

	//examples:random:simple cane
	randomSimpleCaneAction = new QAction("&Simple Cane", this);
	randomSimpleCaneAction->setToolTip("Randomly generate a simple example cane.");

	//examples:random:simple piece
	randomSimplePieceAction = new QAction("&Simple Piece", this);
	randomSimplePieceAction->setToolTip("Randomly generate a simple example piece.");

	//examples:random:complex cane
	randomComplexCaneAction = new QAction("&Complex Cane", this);
	randomComplexCaneAction->setToolTip("Ranomly generate a complex example cane.");

	//examples:random:complex piece
	randomComplexPieceAction = new QAction("&Complex Piece", this);
	randomComplexPieceAction->setToolTip("Randomly generate a complex example piece.");

	// Examples menu and Examples:Random menu
	examplesMenu = menuBar()->addMenu("&Examples"); //create menu for cane/piece examples
	examplesMenu->addAction(randomSimpleCaneAction);
	examplesMenu->addAction(randomComplexCaneAction);
	examplesMenu->addSeparator();
	examplesMenu->addAction(randomSimplePieceAction);
	examplesMenu->addAction(randomComplexPieceAction);

	// toggle depth peeling
	depthPeelAction = new QAction("GPU Transparency", this);
	depthPeelAction->setCheckable(true);
	depthPeelAction->setChecked(GlobalDepthPeelingSetting::enabled());
	depthPeelAction->setToolTip(tr("Toggle transparency in 3D views. Turn off for better framerate."));

	// Performance menu
	perfMenu = menuBar()->addMenu("Performance");
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
			pullPlanEditorWidget->writePlanToOBJFile(userSpecifiedFilename);
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
			pullPlanEditorWidget->writePlanToPLYFile(userSpecifiedFilename);
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
	PullPlan *newEditorPlan = new PullPlan(PullTemplate::BASE_SQUARE);
	if (!SVG::load_svg(userSpecifiedFilename.toUtf8().constData(), svg, newEditorPlan)) 
	{
		QMessageBox::warning(this, "Import failed", "Failed to read " + userSpecifiedFilename);
		deep_delete(newEditorPlan);
		return;
	}

	if (svg.page.c[0] != svg.page.c[1]) 
	{
		QMessageBox::warning(this, "Import failed", 
			"The image in " + userSpecifiedFilename + " is not square.");
		deep_delete(newEditorPlan);
		return;
	}

	pullPlanLibraryLayout->addWidget(new PullPlanLibraryWidget(newEditorPlan, this));
	setViewMode(PULLPLAN_VIEW_MODE);
	pullPlanEditorWidget->setPullPlan(newEditorPlan);
}

void MainWindow::getDependantLibraryContents(Piece* piece, vector<GlassColor*>& colors, vector<PullPlan*>& plans, 
	vector<Piece*>& pieces)
{
	for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
	{
		GlassColorLibraryWidget *cblw = dynamic_cast<GlassColorLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
		if (piece->hasDependencyOn(cblw->glassColor))
		{
			colors.push_back(cblw->glassColor);
			plans.push_back(cblw->circlePlan);
			plans.push_back(cblw->squarePlan);
		}
	}

	for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
	{
		PullPlan* plan = dynamic_cast<PullPlanLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget())->pullPlan;
		if (piece->hasDependencyOn(plan))
			plans.push_back(plan);
	}
	
	pieces.push_back(piece);	
}

void MainWindow::getDependantLibraryContents(PullPlan* plan, vector<GlassColor*>& colors, vector<PullPlan*>& plans)
{
	for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
	{
		GlassColorLibraryWidget *cblw = dynamic_cast<GlassColorLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
		if (plan->hasDependencyOn(cblw->glassColor))
		{
			colors.push_back(cblw->glassColor);
			plans.push_back(cblw->circlePlan);
			plans.push_back(cblw->squarePlan);
		}
	}

	for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
	{
		PullPlan* otherPlan = dynamic_cast<PullPlanLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget())->pullPlan;
		if (plan->hasDependencyOn(otherPlan))
			plans.push_back(otherPlan);
	}
}

void MainWindow::getDependantLibraryContents(GlassColor* color, vector<GlassColor*>& colors, vector<PullPlan*>& plans)
{
	for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
	{
		GlassColorLibraryWidget *cblw = dynamic_cast<GlassColorLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
		if (cblw->glassColor == color)
		{
			colors.push_back(cblw->glassColor);
			plans.push_back(cblw->circlePlan);
			plans.push_back(cblw->squarePlan);
			break;
		}
	}
}

void MainWindow::getLibraryContents(vector<GlassColor*>& colors, vector<PullPlan*>& plans, vector<Piece*>& pieces)
{
	for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
	{
		GlassColorLibraryWidget *cblw = dynamic_cast<GlassColorLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
		colors.push_back(cblw->glassColor);
		plans.push_back(cblw->circlePlan);
		plans.push_back(cblw->squarePlan);
	}

	for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
	{
		PullPlanLibraryWidget* pplw = dynamic_cast<PullPlanLibraryWidget*>(
			dynamic_cast<QWidgetItem*>(pullPlanLibraryLayout->itemAt(i))->widget());
		plans.push_back(pplw->pullPlan);
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
	colorEditorWidget->resetGlassColor();
	pullPlanEditorWidget->resetPullPlan();
	pieceEditorWidget->resetPiece();

	// 2. delete everything in the library
	clearLibrary();

	// 3. add the three new guys from the editors into the library 
	colorBarLibraryLayout->addWidget(new GlassColorLibraryWidget(colorEditorWidget->getGlassColor(), this));
	pullPlanLibraryLayout->addWidget(new PullPlanLibraryWidget(pullPlanEditorWidget->getPullPlan(), this));
	pieceLibraryLayout->addWidget(new PieceLibraryWidget(pieceEditorWidget->getPiece(), this)); 

	// 4. go back to empty view mode
	setViewMode(EMPTY_VIEW_MODE);

	// reset filename
	setSaveFilename("[unsaved]");
	setDirtyBit(false);
}

void MainWindow::addToLibrary(vector<GlassColor*>& colors, vector<PullPlan*>& plans, vector<Piece*>& pieces)
{
	for (unsigned int i = 0; i < colors.size(); ++i)
	{
		// search ad hoc for plans that match the description of being 
		// the type of plans that live in color bar library widgets, and 
		// have the right color
		// 
		// this approach is biased *towards* sucking plans into color bar
		// library widgets. if you actually make pull plans that look exactly
		// like color bars (no subplans, one casing), it will rip the first one
		// out of the pull plan list (where you had it) and put it in the 
		// corresponding color library widget. 
		//
		// preserving the library exactly in all cases would require explicitly 
		// writing membership in the save file, either in colors or pull plans.
		// we don't do this for now to keep the save files as simple as possible,
		// and bias the user's mental model towards "color bars live in color
		// objects" and away from "I need to make a new color bar pull plan to
		// use a color". 
		//
		// if the json file is never hand edited and the plans have the same label
		// ordering as that induced when writing to a file, then save/load preserves
		// the library exactly
		PullPlan* circlePlan = NULL;
		for (unsigned int j = 0; j < plans.size(); ++j)
		{
			// we take the *first* matching plan because in getLibraryContents()
			// we add the color library widget plans to the plans list *first*.
			if (plans[j]->getTemplateType() == PullTemplate::BASE_CIRCLE 
				&& plans[j]->getCasingCount() == 1
				&& plans[j]->getOutermostCasingColor() == colors[i])
			{
				circlePlan = plans[j];
				plans.erase(plans.begin() + j);
				break;
			}
		}
		PullPlan* squarePlan = NULL;
		for (unsigned int j = 0; j < plans.size(); ++j)
		{
			if (plans[j]->getTemplateType() == PullTemplate::BASE_SQUARE 
				&& plans[j]->getCasingCount() == 1
				&& plans[j]->getOutermostCasingColor() == colors[i])
			{
				squarePlan = plans[j];
				plans.erase(plans.begin() + j);
				break;
			}
		}
		colorBarLibraryLayout->addWidget(new GlassColorLibraryWidget(colors[i], this, 
			circlePlan, squarePlan));
	}
	for (unsigned int i = 0; i < plans.size(); ++i)
		pullPlanLibraryLayout->addWidget(new PullPlanLibraryWidget(plans[i], this));
	for (unsigned int i = 0; i < pieces.size(); ++i)
		pieceLibraryLayout->addWidget(new PieceLibraryWidget(pieces[i], this));

}

void MainWindow::openFile(QString filename, bool add)
{
	// try to read in the file
	vector<GlassColor*> colors;
	vector<PullPlan*> plans;
	vector<Piece*> pieces;
	bool success = readGlassFile(filename, colors, plans, pieces);

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
	
	addToLibrary(colors, plans, pieces);	
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

void MainWindow::shareFileActionTriggered()
{
	bool ok;
	QString userSpecifiedAddress = QInputDialog::getText(this, "Email your design", "Email address:", 
		QLineEdit::Normal, "friend@internet.com", &ok);
	
	// Check for basic validity
	if (!ok || userSpecifiedAddress.isEmpty()) 
		return;

	// Check for email address validity?
	if (!userSpecifiedAddress.contains("@"))
	{
		QMessageBox::warning(this, "Invalid email address", "The address " + userSpecifiedAddress + " is invalid.");
		return;	
	}

	
	// Step 1. Grab selected object and dependancies and write them to a temp file
	vector<GlassColor*> colors;
	vector<PullPlan*> plans;
	vector<Piece*> pieces;
	switch (editorStack->currentIndex())
	{
		case EMPTY_VIEW_MODE:
			return; // nothing to save
		case COLORBAR_VIEW_MODE:
			getDependantLibraryContents(colorEditorWidget->getGlassColor(), colors, plans);
			break;
		case PULLPLAN_VIEW_MODE:
			getDependantLibraryContents(pullPlanEditorWidget->getPullPlan(), colors, plans);
			break;
		case PIECE_VIEW_MODE:
			getDependantLibraryContents(pieceEditorWidget->getPiece(), colors, plans, pieces);
			break;
	}
	writeGlassFile("./tmp.glass", colors, plans, pieces);	


	// Step 2. Try to send the email
	// TODO: Erik will make magic happen here
	bool success = true;
    Email *email = new Email(userSpecifiedAddress, QString("shared design"));
    email->attachGlass("./tmp.glass");
    email->attachImage("./tmp.glass", "glassmage");
	email->send();

	// Step 3. Report a success/error message?	
	if (success)
		QMessageBox::information(this, "Email sent!", "Your design has been sent to " + userSpecifiedAddress + ".");
	else
		QMessageBox::warning(this, "Email failed", "Failed to send your design to " + userSpecifiedAddress + ".");
}

void MainWindow::saveAllFileActionTriggered()
{
	if (saveFilename == "[unsaved]")
		saveAllAsFileActionTriggered();
	else
	{
		// call the actual file-saving code in GlassFileWriter
		vector<GlassColor*> colors;
		vector<PullPlan*> colorPlans;
		vector<PullPlan*> plans;
		vector<Piece*> pieces;
		getLibraryContents(colors, plans, pieces);
		writeGlassFile(saveFilename, colors, plans, pieces);	
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
	vector<PullPlan*> plans;
	vector<Piece*> pieces;
	getLibraryContents(colors, plans, pieces);
	writeGlassFile(saveFilename, colors, plans, pieces);	
	setDirtyBit(false);
}

void MainWindow::saveSelectedAsFileActionTriggered()
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

	vector<GlassColor*> colors;
	vector<PullPlan*> plans;
	vector<Piece*> pieces;
	switch (editorStack->currentIndex())
	{
		case EMPTY_VIEW_MODE:
			return; // nothing to save
		case COLORBAR_VIEW_MODE:
			getDependantLibraryContents(colorEditorWidget->getGlassColor(), colors, plans);
			break;
		case PULLPLAN_VIEW_MODE:
			getDependantLibraryContents(pullPlanEditorWidget->getPullPlan(), colors, plans);
			break;
		case PIECE_VIEW_MODE:
			getDependantLibraryContents(pieceEditorWidget->getPiece(), colors, plans, pieces);
			break;
	}
	
	// pass off "curated" library to regular write file; 
	// effectively pretending that the library only has your thing and its dependancies
	writeGlassFile(userSpecifiedFilename, colors, plans, pieces);	

	// this doesn't impact dirty bit or saveFilename at all: it's a special operation that 
	// virtualglass has that lives outside of the usual file-editor relationship, e.g. of a text editor. 
}




