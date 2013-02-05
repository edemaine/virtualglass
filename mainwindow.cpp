
#include <QtGui>
#include <QFileDialog>

#include "constants.h"
#include "dependancy.h"
#include "niceviewwidget.h"
#include "asyncpiecelibrarywidget.h"
#include "asyncpullplanlibrarywidget.h"
#include "asynccolorbarlibrarywidget.h"
#include "pullplan.h"
#include "pulltemplate.h"
#include "pickupplan.h"
#include "piece.h"
#include "pullplaneditorwidget.h"
#include "coloreditorwidget.h"
#include "pieceeditorwidget.h"
#include "randomglass.h"
#include "glassmime.h"
#include "exampleglass.h"
#include "pulltemplate.h"
#include "mainwindow.h"
#include "globalglass.h"
#include "SVG.hpp"
#include "glassfilewriter.h"
#include "glassfilereader.h"

MainWindow :: MainWindow()
{
	centralWidget = new QWidget(this);
	this->setCentralWidget(centralWidget);

	centralLayout = new QHBoxLayout(centralWidget);
	setupLibrary();
	setupEditors();
	setupMenus();
	setupSaveFile();
	setupConnections();
	setWindowTitle(windowTitle());
	seedEverything();

	setViewMode(EMPTY_VIEW_MODE);
	show();
	setDirtyBit(false);

	//emit someDataChanged();
}


void MainWindow :: setViewMode(enum ViewMode _mode)
{
	editorStack->setCurrentIndex(_mode);
	copyColorBarButton->setEnabled(false);
	copyPullPlanButton->setEnabled(false);
	copyPieceButton->setEnabled(false);
	switch (_mode)
	{
		case EMPTY_VIEW_MODE:
			// leave all copy buttons disabled
			break;
		case COLORBAR_VIEW_MODE:
			copyColorBarButton->setEnabled(true);
			break;
		case PULLPLAN_VIEW_MODE:
			copyPullPlanButton->setEnabled(true);
			break;
		case PIECE_VIEW_MODE:
			copyPieceButton->setEnabled(true);
			break;
	}
	emit someDataChanged();
}

QString MainWindow :: windowTitle()
{
	QString title = tr("VirtualGlass");
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

void MainWindow :: seedEverything()
{
	// Load color stuff
	setViewMode(COLORBAR_VIEW_MODE);
	emit someDataChanged();

	// Load pull template types
	setViewMode(PULLPLAN_VIEW_MODE);
	pullPlanEditorWidget->seedTemplates();

	// Load pickup and piece template types
	setViewMode(PIECE_VIEW_MODE);
	pieceEditorWidget->seedTemplates();
}

void MainWindow :: unhighlightAllLibraryWidgets()
{
	QLayoutItem* w;
	for (int j = 0; j < colorBarLibraryLayout->count(); ++j)
	{
		w = colorBarLibraryLayout->itemAt(j);
		static_cast<AsyncColorBarLibraryWidget*>(w->widget())->setDependancy(false);
	}
	for (int j = 0; j < pullPlanLibraryLayout->count(); ++j)
	{
		w = pullPlanLibraryLayout->itemAt(j);
		static_cast<AsyncPullPlanLibraryWidget*>(w->widget())->setDependancy(false);
	}
	for (int j = 0; j < pieceLibraryLayout->count(); ++j)
	{
		w = pieceLibraryLayout->itemAt(j);
		static_cast<AsyncPieceLibraryWidget*>(w->widget())->setDependancy(false);
	}
}

void MainWindow :: keyPressEvent(QKeyEvent* e)
{
	if (e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete)
		deleteCurrentEditingObject();
}

void MainWindow :: deleteCurrentEditingObject()
{
	QLayoutItem* w;

	switch (editorStack->currentIndex())
	{
		case COLORBAR_VIEW_MODE:
		{
			if (colorBarLibraryLayout->count() == 1)
				return;

			int i;
			for (i = 0; i < colorBarLibraryLayout->count(); ++i)
			{
				w = colorBarLibraryLayout->itemAt(i);
				GlassColor* gc = dynamic_cast<AsyncColorBarLibraryWidget*>(w->widget())->getGlassColor();
				if (gc == colorEditorWidget->getGlassColor())
				{
					if (glassColorIsDependancy(gc))
					{
						QMessageBox msgBox;
						msgBox.setText("This color cannot be deleted: other objects use it.");
						msgBox.exec();
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

			colorEditorWidget->setGlassColor(dynamic_cast<AsyncColorBarLibraryWidget*>(colorBarLibraryLayout->itemAt(
					MIN(colorBarLibraryLayout->count()-1, i))->widget())->getGlassColor());
			emit someDataChanged();
			break;
		}
		case PULLPLAN_VIEW_MODE:
		{
			if (pullPlanLibraryLayout->count() == 1)
				return;

			int i;
			for (i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				w = pullPlanLibraryLayout->itemAt(i);
				PullPlan* p = dynamic_cast<AsyncPullPlanLibraryWidget*>(w->widget())->getPullPlan();
				if (p == pullPlanEditorWidget->getPlan())
				{
					if (pullPlanIsDependancy(p))
					{
						QMessageBox msgBox;
						msgBox.setText("This cane cannot be deleted: other objects use it.");
						msgBox.exec();
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

			pullPlanEditorWidget->setPlan(dynamic_cast<AsyncPullPlanLibraryWidget*>(pullPlanLibraryLayout->itemAt(
					MIN(pullPlanLibraryLayout->count()-1, i))->widget())->getPullPlan());
			emit someDataChanged();
			break;
		}
		case PIECE_VIEW_MODE:
		{
			if (pieceLibraryLayout->count() == 1)
				return;

			int i;
			for (i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				w = pieceLibraryLayout->itemAt(i);
				Piece* p = dynamic_cast<AsyncPieceLibraryWidget*>(w->widget())->getPiece();
				if (p == pieceEditorWidget->getPiece())
				{
					// this may be a memory leak, the library widget is never explicitly deleted
					w = pieceLibraryLayout->takeAt(i);
					delete w->widget();
					delete w;
					break;
				}
			}

			pieceEditorWidget->setPiece(dynamic_cast<AsyncPieceLibraryWidget*>(pieceLibraryLayout->itemAt(
					MIN(pieceLibraryLayout->count()-1, i))->widget())->getPiece());
			emit someDataChanged();
			break;
		}
	}

}

void MainWindow :: mouseReleaseEvent(QMouseEvent* event)
{
	// If this is a drag and not the end of a click, don't process (dropEvent will do it instead)
	if (isDragging && (event->pos() - dragStartPosition).manhattanLength() > QApplication::startDragDistance()) return;

	AsyncColorBarLibraryWidget* cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(childAt(event->pos()));
	AsyncPullPlanLibraryWidget* plplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(childAt(event->pos()));
	AsyncPieceLibraryWidget* plw = dynamic_cast<AsyncPieceLibraryWidget*>(childAt(event->pos()));

	if (cblw != NULL)
	{
		colorEditorWidget->setGlassColor(cblw->getGlassColor());
		setViewMode(COLORBAR_VIEW_MODE);
	}
	else if (plplw != NULL)
	{
		pullPlanEditorWidget->setPlan(plplw->getPullPlan());
		setViewMode(PULLPLAN_VIEW_MODE);
	}
	else if (plw != NULL)
	{
		pieceEditorWidget->setPiece(plw->getPiece());
		setViewMode(PIECE_VIEW_MODE);
	}
}


void MainWindow :: mousePressEvent(QMouseEvent* event)
{
	AsyncColorBarLibraryWidget* cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(childAt(event->pos()));
	AsyncPullPlanLibraryWidget* plplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(childAt(event->pos()));

	if (event->button() == Qt::LeftButton && (cblw != NULL || plplw != NULL))
	{
		isDragging = true;
		this->dragStartPosition = event->pos();
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

	if (!isDragging || (event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
		return;

	AsyncColorBarLibraryWidget* cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(childAt(event->pos()));
	AsyncPullPlanLibraryWidget* plplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(childAt(event->pos()));

	char buf[500];
	QPixmap pixmap;

	if (cblw != NULL)
	{
		GlassMime::encode(buf, cblw->getGlassColor()->getColor(), GlassMime::colorbar);
		pixmap = *cblw->getDragPixmap();
	}
	else if (plplw != NULL)
	{
		GlassMime::encode(buf, plplw->getPullPlan(), GlassMime::pullplan);
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

	drag->exec(Qt::CopyAction);
}

void MainWindow :: setupConnections()
{
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

	connect(newColorBarButton, SIGNAL(pressed()), this, SLOT(newColorBar()));
	connect(copyColorBarButton, SIGNAL(pressed()), this, SLOT(copyColorBar()));
	connect(colorEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

	connect(newPullPlanButton, SIGNAL(pressed()), this, SLOT(newPullPlan()));
	connect(copyPullPlanButton, SIGNAL(pressed()), this, SLOT(copyPullPlan()));
	connect(pullPlanEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

	connect(newPieceButton, SIGNAL(pressed()), this, SLOT(newPiece()));
	connect(copyPieceButton, SIGNAL(pressed()), this, SLOT(copyPiece()));
	connect(pieceEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

	// the file IO menu stuff
	connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));
	connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(addAct, SIGNAL(triggered()), this, SLOT(addFile()));
	connect(importSVGAct, SIGNAL(triggered()), this, SLOT(importSVG()));
	connect(saveAllAct, SIGNAL(triggered()), this, SLOT(saveAllFile()));
	connect(saveAllAsAct, SIGNAL(triggered()), this, SLOT(saveAllAsFile()));
	connect(saveSelectedAsAct, SIGNAL(triggered()), this, SLOT(saveSelectedAsFile()));
	connect(exitAct, SIGNAL(triggered()), qApp, SLOT(quit()));

	connect(randomSimpleCaneAction, SIGNAL(triggered()), this, SLOT(randomSimpleCaneExampleActionTriggered()));
	connect(randomSimplePieceAction, SIGNAL(triggered()), this, SLOT(randomSimplePieceExampleActionTriggered()));

	connect(randomComplexCaneAction, SIGNAL(triggered()), this, SLOT(randomComplexCaneExampleActionTriggered()));
	connect(randomComplexPieceAction, SIGNAL(triggered()), this, SLOT(randomComplexPieceExampleActionTriggered()));

	connect(web1PieceAction, SIGNAL(triggered()), this, SLOT(web1PieceExampleActionTriggered()));
	connect(web2PieceAction, SIGNAL(triggered()), this, SLOT(web2PieceExampleActionTriggered()));

	connect(depthPeelAction, SIGNAL(triggered()), this, SLOT(depthPeelActionTriggered()));
}

void MainWindow :: depthPeelActionTriggered()
{
	NiceViewWidget::peelEnable = !(NiceViewWidget::peelEnable);
	depthPeelAction->setChecked(NiceViewWidget::peelEnable);
	emit someDataChanged();
}

void MainWindow :: web1PieceExampleActionTriggered()
{
	GlassColor* gc;
	PullPlan *pp;
	Piece* p;

	web1Piece(&gc, &pp, &p);

	colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(gc));
	pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(pp));
	pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(p));

	pieceEditorWidget->setPiece(p);
	setViewMode(PIECE_VIEW_MODE);
}

void MainWindow :: web2PieceExampleActionTriggered()
{
	GlassColor* gc;
	PullPlan *pp1, *pp2;
	Piece* p;

	web2Piece(&gc, &pp1, &pp2, &p);

	colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(gc));
	pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(pp1));
	pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(pp2));
	pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(p));

	pieceEditorWidget->setPiece(p);
	setViewMode(PIECE_VIEW_MODE);
}

void MainWindow :: randomSimpleCaneExampleActionTriggered()
{
	GlassColor* randomGC = randomGlassColor();
	PullPlan* randomPP = randomSimplePullPlan(CIRCLE_SHAPE, randomGC);

	colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(randomGC));

	AsyncPullPlanLibraryWidget* pplw = new AsyncPullPlanLibraryWidget(randomPP);
	pullPlanLibraryLayout->addWidget(pplw);

	pullPlanEditorWidget->setPlan(randomPP);
	setViewMode(PULLPLAN_VIEW_MODE);
}

void MainWindow :: randomComplexCaneExampleActionTriggered()
{
	GlassColor* randomGC = randomGlassColor();
	PullPlan* randomCPP = randomSimplePullPlan(CIRCLE_SHAPE, randomGC);
	PullPlan* randomSPP = randomSimplePullPlan(SQUARE_SHAPE, randomGC);
	PullPlan* randomComplexPP = randomComplexPullPlan(randomCPP, randomSPP);

	colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(randomGC));
	pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomComplexPP));
	// add simple plans only if they are used
	// memory leak! as unused ones never appear in library
	if (randomComplexPP->hasDependencyOn(randomCPP))
		pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomCPP));
	if (randomComplexPP->hasDependencyOn(randomSPP))
		pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomSPP));

	pullPlanEditorWidget->setPlan(randomComplexPP);
	setViewMode(PULLPLAN_VIEW_MODE);
}

void MainWindow :: randomSimplePieceExampleActionTriggered()
{
	GlassColor* randomGC = randomGlassColor();
	PullPlan* randomSPP = randomSimplePullPlan(SQUARE_SHAPE, randomGC);
	Piece* randomP = randomPiece(randomPickup(randomSPP));

	colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(randomGC));
	pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomSPP));

	AsyncPieceLibraryWidget* plw = new AsyncPieceLibraryWidget(randomP);
	pieceLibraryLayout->addWidget(plw);

	pieceEditorWidget->setPiece(randomP);
	setViewMode(PIECE_VIEW_MODE);
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
		colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(randomGC1));
	if (randomP->hasDependencyOn(randomGC2)) // memory leak if returns no
		colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(randomGC2));
	if (randomP->hasDependencyOn(randomCPP)) // memory leak if returns no
		pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomCPP));
	if (randomP->hasDependencyOn(randomSPP)) // memory leak if returns no
		pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomSPP));
	pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomComplexPP1));
	pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomComplexPP2));
	pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(randomP));

	pieceEditorWidget->setPiece(randomP);
	setViewMode(PIECE_VIEW_MODE);
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
	QWidget* bigOleLibraryWidget = new QWidget(centralWidget);
	centralLayout->addWidget(bigOleLibraryWidget);

	QVBoxLayout* libraryAreaLayout = new QVBoxLayout(bigOleLibraryWidget);
	bigOleLibraryWidget->setLayout(libraryAreaLayout);

	QScrollArea* libraryScrollArea = new QScrollArea(bigOleLibraryWidget);
	libraryAreaLayout->addWidget(libraryScrollArea, 1);
	libraryScrollArea->setBackgroundRole(QPalette::Dark);
	libraryScrollArea->setWidgetResizable(true);
	libraryScrollArea->setFixedWidth(370);
	libraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	libraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	QWidget* libraryWidget = new QWidget(libraryScrollArea);
	libraryScrollArea->setWidget(libraryWidget);

	QGridLayout* superlibraryLayout = new QGridLayout(libraryWidget);
	libraryWidget->setLayout(superlibraryLayout);

	newColorBarButton = new QPushButton("New Color", libraryWidget);
	newPullPlanButton = new QPushButton("New Cane", libraryWidget);
	newPieceButton = new QPushButton("New Piece", libraryWidget);
	superlibraryLayout->addWidget(newColorBarButton, 0, 0);
	superlibraryLayout->addWidget(newPullPlanButton, 0, 1);
	superlibraryLayout->addWidget(newPieceButton, 0, 2);

	copyColorBarButton = new QPushButton("Copy Color", libraryWidget);
	copyPullPlanButton = new QPushButton("Copy Cane", libraryWidget);
	copyPieceButton = new QPushButton("Copy Piece", libraryWidget);
	superlibraryLayout->addWidget(copyColorBarButton, 1, 0);
	superlibraryLayout->addWidget(copyPullPlanButton, 1, 1);
	superlibraryLayout->addWidget(copyPieceButton, 1, 2);

	colorBarLibraryLayout = new QVBoxLayout(libraryWidget);
	colorBarLibraryLayout->setDirection(QBoxLayout::BottomToTop);
	pullPlanLibraryLayout = new QVBoxLayout(libraryWidget);
	pullPlanLibraryLayout->setDirection(QBoxLayout::BottomToTop);
	pieceLibraryLayout = new QVBoxLayout(libraryWidget);
	pieceLibraryLayout->setDirection(QBoxLayout::BottomToTop);
	superlibraryLayout->addLayout(colorBarLibraryLayout, 2, 0, Qt::AlignTop);
	superlibraryLayout->addLayout(pullPlanLibraryLayout, 2, 1, Qt::AlignTop);
	superlibraryLayout->addLayout(pieceLibraryLayout, 2, 2, Qt::AlignTop);

	// make three qlabels for a legend
	QGridLayout* legendLayout = new QGridLayout(libraryWidget);
	QLabel* l1 = new QLabel("Used By Selected");
	l1->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	l1->setStyleSheet("border: 2px dashed " + QColor(0, 139, 69, 255).name() + ";");
	QLabel* l2 = new QLabel("Selected");
	l2->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	l2->setStyleSheet("border: 2px solid " + QColor(0, 0, 255, 255).name() + ";");
	QLabel* l3 = new QLabel("Uses Selected");
	l3->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	l3->setStyleSheet("border: 2px dotted " + QColor(200, 100, 0, 255).name() + ";");
	legendLayout->addWidget(l1,0,1);
	legendLayout->addWidget(l2,0,2);
	legendLayout->addWidget(l3,0,3);
	libraryAreaLayout->addLayout(legendLayout, 0);

	QLabel* descriptionLabel = new QLabel("Library - click to edit or drag to add.",
		libraryWidget);
	descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	libraryAreaLayout->addWidget(descriptionLabel, 0);
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
	setupEmptyPaneEditor();
	editorStack->addWidget(emptyEditorPage);

	setupColorEditor();
	editorStack->addWidget(colorEditorWidget);

	setupPullPlanEditor();
	editorStack->addWidget(pullPlanEditorWidget);

	setupPieceEditor();
	editorStack->addWidget(pieceEditorWidget);
}

void MainWindow :: setupEmptyPaneEditor()
{
	emptyEditorPage = new QWidget(editorStack);
	QHBoxLayout* editorLayout = new QHBoxLayout(emptyEditorPage);
	emptyEditorPage->setLayout(editorLayout);
	whatToDoLabel = new QLabel("Click a library item at left to edit/view.", emptyEditorPage);
	whatToDoLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	editorLayout->addWidget(whatToDoLabel, 0);
}

void MainWindow :: setupColorEditor()
{
	// Setup data objects - the current plan and library widget for this plan
	colorEditorWidget = new ColorEditorWidget(editorStack);
	colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(colorEditorWidget->getGlassColor(), this));
}

void MainWindow :: setupPullPlanEditor()
{
	// Setup data objects - the current plan and library widget for this plan
	pullPlanEditorWidget = new PullPlanEditorWidget(editorStack);
	pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(pullPlanEditorWidget->getPlan()));
}

void MainWindow :: setupPieceEditor()
{
	// Setup data objects - the current plan and library widget for this plan
	pieceEditorWidget = new PieceEditorWidget(editorStack);
	pieceEditorWidget->setMainWindow(this);
	pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(pieceEditorWidget->getPiece()));
}

void MainWindow :: newPiece()
{
	// Create the new piece
	Piece* newEditorPiece = new Piece(PieceTemplate::TUMBLER);

	// Create the new library entry
	pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(newEditorPiece));
	pieceEditorWidget->setPiece(newEditorPiece);

	// Load up the right editor
	setViewMode(PIECE_VIEW_MODE);
}

void MainWindow :: copyPiece()
{
	if (editorStack->currentIndex() != PIECE_VIEW_MODE)
		return;

	// Create the new piece
	Piece* newEditorPiece = pieceEditorWidget->getPiece()->copy();
	pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(newEditorPiece));
	pieceEditorWidget->setPiece(newEditorPiece);

	emit someDataChanged();
}

PickupPlan* MainWindow :: newPickupPlan()
{
	return new PickupPlan(PickupTemplate::VERTICAL);
}

void MainWindow :: newColorBar()
{
	GlassColor* newGlassColor = new GlassColor();

	// Create the new library entry
	colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(newGlassColor, this));
	colorEditorWidget->setGlassColor(newGlassColor);

	// Load up the right editor
	setViewMode(COLORBAR_VIEW_MODE);
}

void MainWindow :: copyColorBar()
{
	if (editorStack->currentIndex() != COLORBAR_VIEW_MODE)
		return;

	GlassColor* newEditorGlassColor = colorEditorWidget->getGlassColor()->copy();

	// Create the new library entry
	colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(newEditorGlassColor, this));
	colorEditorWidget->setGlassColor(newEditorGlassColor);

	// Trigger GUI updates
	emit someDataChanged();
}

void MainWindow :: newPullPlan()
{
	PullPlan *newEditorPlan = new PullPlan(PullTemplate::HORIZONTAL_LINE_CIRCLE);
	emit newPullPlan(newEditorPlan);
}

void MainWindow :: copyPullPlan()
{
	if (editorStack->currentIndex() != PULLPLAN_VIEW_MODE)
		return;

	PullPlan *newEditorPlan = pullPlanEditorWidget->getPlan()->copy();
	emit newPullPlan(newEditorPlan);
}

void MainWindow :: newPullPlan(PullPlan* newPlan)
{
	pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(newPlan));

	// Give the new plan to the editor
    pullPlanEditorWidget->setPlan(newPlan);

	// Load up the right editor
	setViewMode(PULLPLAN_VIEW_MODE);
}

void MainWindow :: updateEverything()
{
	setDirtyBit(1); // why are we updating? something probably changed...
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
	updateLibrary();
}

// returns whether the pull plan is a dependancy of something in the library
// (either a pull plan or a piece)
bool MainWindow :: glassColorIsDependancy(GlassColor* color)
{
	AsyncPullPlanLibraryWidget* pplw;
	for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
	{
		pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
		if (pplw->getPullPlan()->hasDependencyOn(color))
		{
			return true;
		}
	}

	AsyncPieceLibraryWidget* plw;
	for (int i = 0; i < pieceLibraryLayout->count(); ++i)
	{
		plw = dynamic_cast<AsyncPieceLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
		if (plw->getPiece()->hasDependencyOn(color))
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
	AsyncPullPlanLibraryWidget* pplw;
	for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
	{
		pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
		// Check whether the pull plan in the library is:
		// 1. the parameter plan
		// 2. a plan with the plan currently being edited as a subplan
		if (pplw->getPullPlan() == plan)
		{
			continue;
		}
		else if (pplw->getPullPlan()->hasDependencyOn(plan))
		{
			return true;
		}
	}

	AsyncPieceLibraryWidget* plw;
	for (int i = 0; i < pieceLibraryLayout->count(); ++i)
	{
		plw = dynamic_cast<AsyncPieceLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
		if (plw->getPiece()->hasDependencyOn(plan))
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
			AsyncColorBarLibraryWidget* cblw;
			for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
			{
				cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(
						dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
				if (colorEditorWidget->getGlassColor() == cblw->getGlassColor())
				{
					cblw->updatePixmaps();
					cblw->setDependancy(true, IS_DEPENDANCY);
				}
			}


			AsyncPullPlanLibraryWidget* pplw;
			for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
				if (pplw->getPullPlan()->hasDependencyOn(colorEditorWidget->getGlassColor()))
				{
					pplw->updatePixmaps();
					pplw->setDependancy(true, USES_DEPENDANCY);
				}
			}

			AsyncPieceLibraryWidget* plw;
			for (int i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				plw = dynamic_cast<AsyncPieceLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
				if (plw->getPiece()->hasDependencyOn(colorEditorWidget->getGlassColor()))
				{
					plw->updatePixmap();
					plw->setDependancy(true, USES_DEPENDANCY);
				}
			}

			break;
		}
		case PULLPLAN_VIEW_MODE:
		{
			AsyncColorBarLibraryWidget* cblw;
			for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
			{
				cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
				if (pullPlanEditorWidget->getPlan()->hasDependencyOn(cblw->getGlassColor()))
					cblw->setDependancy(true, USEDBY_DEPENDANCY);
			}

			AsyncPullPlanLibraryWidget* pplw;
			for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
						dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
				// Check whether the pull plan in the library is:
				// 1. the plan currently being edited
				// 2. a subplan of the plan current being edited
				// 3. a plan with the plan currently being edited as a subplan
				if (pullPlanEditorWidget->getPlan() == pplw->getPullPlan())
				{
					pplw->updatePixmaps();
					pplw->setDependancy(true, IS_DEPENDANCY);
				}
				else if (pullPlanEditorWidget->getPlan()->hasDependencyOn(pplw->getPullPlan()))
				{
					pplw->setDependancy(true, USEDBY_DEPENDANCY);
				}
				else if (pplw->getPullPlan()->hasDependencyOn(pullPlanEditorWidget->getPlan()))
				{
					pplw->updatePixmaps();
					pplw->setDependancy(true, USES_DEPENDANCY);
				}
			}

			AsyncPieceLibraryWidget* plw;
			for (int i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				plw = dynamic_cast<AsyncPieceLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
				if (plw->getPiece()->hasDependencyOn(pullPlanEditorWidget->getPlan()))
				{
					plw->updatePixmap();
					plw->setDependancy(true, USES_DEPENDANCY);
				}
			}

			break;
		}
		case PIECE_VIEW_MODE:
		{
			AsyncColorBarLibraryWidget* cblw;
			for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
			{
				cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
				if (pieceEditorWidget->getPiece()->hasDependencyOn(cblw->getGlassColor()))
					cblw->setDependancy(true, USEDBY_DEPENDANCY);
			}

			AsyncPullPlanLibraryWidget* pplw;
			for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
					dynamic_cast<QWidgetItem*>(pullPlanLibraryLayout->itemAt(i))->widget());
				if (pieceEditorWidget->getPiece()->hasDependencyOn(pplw->getPullPlan()))
					pplw->setDependancy(true, USEDBY_DEPENDANCY);
			}

			AsyncPieceLibraryWidget* plw;
			for (int i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				plw = dynamic_cast<AsyncPieceLibraryWidget*>(
						dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
				if (plw->getPiece() == pieceEditorWidget->getPiece())
				{
					plw->updatePixmap();
					plw->setDependancy(true, IS_DEPENDANCY);
				}
			}

			break;
		}
	}
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (!dirtyBit)
	{
		event->accept();
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
			event->ignore();
			return;
		case QMessageBox::Save:
			saveAllFile();
			break;
		case QMessageBox::Discard:
			break;
	}
	event->accept();
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);
	menu.exec(event->globalPos());
}

void MainWindow::setupMenus()
{
	//new
	newAct = new QAction(tr("&New"), this);
	newAct->setShortcuts(QKeySequence::New);
	newAct->setStatusTip(tr("Open a new empty file."));

	//open
	openAct = new QAction(tr("&Open"), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("Open an existing file."));

	//add
	addAct = new QAction(tr("&Add"), this);
	addAct->setStatusTip(tr("Add an existing file."));

	//import svg cane
	importSVGAct = new QAction(tr("&Import SVG Cane"), this);
	importSVGAct->setStatusTip(tr("Import cane cross section from .svg file."));

	//save
	saveAllAct = new QAction(tr("&Save"), this);
	saveAllAct->setShortcuts(QKeySequence::Save);
	saveAllAct->setStatusTip(tr("Save library to file."));

	//saveAllAs
	saveAllAsAct = new QAction(tr("Save &As"), this);
	saveAllAsAct->setShortcuts(QKeySequence::SaveAs);
	saveAllAsAct->setStatusTip(tr("Save library to file."));

	//saveSelectedAs
	saveSelectedAsAct = new QAction(tr("Save Selected As"), this);
	saveSelectedAsAct->setStatusTip(tr("Save selected object to file."));

	//exit
	exitAct = new QAction(tr("Q&uit"), this);
	exitAct->setShortcuts(QKeySequence::Quit);
	exitAct->setStatusTip(tr("Quit"));

	//File menu
	fileMenu = menuBar()->addMenu(tr("&File")); //create File menu
	fileMenu->addAction(newAct); //add newButton
	fileMenu->addAction(openAct); //add openButton
	fileMenu->addAction(addAct); //add addButton
	fileMenu->addSeparator();
	fileMenu->addAction(importSVGAct); //add importButton
	fileMenu->addSeparator();
	fileMenu->addAction(saveAllAct); //add saveButton
	fileMenu->addAction(saveAllAsAct); //add saveButton
	fileMenu->addAction(saveSelectedAsAct); //add saveAsButton
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	//examples:webtutorial1
	web1PieceAction = new QAction("Tutorial 1", this);
	web1PieceAction->setStatusTip("The first tutorial found on virtualglass.org");

	//examples:webtutorial2
	web2PieceAction = new QAction("Tutorial 2", this);
	web2PieceAction->setStatusTip("The second tutorial found on virtualglass.org");

	//examples:random:simple cane
	randomSimpleCaneAction = new QAction("&Simple Cane", this);
	randomSimpleCaneAction->setStatusTip("Randomly generate a simple example cane.");

	//examples:random:simple piece
	randomSimplePieceAction = new QAction("&Simple Piece", this);
	randomSimplePieceAction->setStatusTip("Randomly generate a simple example piece.");

	//examples:random:complex cane
	randomComplexCaneAction = new QAction("&Complex Cane", this);
	randomComplexCaneAction->setStatusTip("Ranomly generate a complex example cane.");

	//examples:random:complex piece
	randomComplexPieceAction = new QAction("&Complex Piece", this);
	randomComplexPieceAction->setStatusTip("Randomly generate a complex example piece.");

	// Examples menu and Examples:Random menu
	examplesMenu = menuBar()->addMenu("&Examples"); //create menu for cane/piece examples
	webExamplesMenu = examplesMenu->addMenu("&Web");
	webExamplesMenu->addAction(web1PieceAction);
	webExamplesMenu->addAction(web2PieceAction);
	randomExamplesMenu = examplesMenu->addMenu("&Random");
	randomExamplesMenu->addAction(randomSimpleCaneAction);
	randomExamplesMenu->addAction(randomComplexCaneAction);
	randomExamplesMenu->addSeparator();
	randomExamplesMenu->addAction(randomSimplePieceAction);
	randomExamplesMenu->addAction(randomComplexPieceAction);

	// toggle depth peeling
	depthPeelAction = new QAction(tr("&Depth peeling"), this);
	depthPeelAction->setCheckable(true);
	depthPeelAction->setChecked(NiceViewWidget::peelEnable);
	depthPeelAction->setStatusTip(tr("Toggle high-quality transparency rendering in 3D views"));

	// Performance menu
	perfMenu = menuBar()->addMenu(tr("Performance"));
	perfMenu->addAction(depthPeelAction);

}

void MainWindow::importSVG()
{
	QFileDialog importFileDialog(this);
	importFileDialog.setOption(QFileDialog::DontUseNativeDialog);
	importFileDialog.setWindowTitle(tr("Open your SVG cane crosssection file"));
	importFileDialog.setNameFilter(tr("Scalable Vector Graphics (*.svg)")); //avoid open non .svg files
	importFileDialog.setFileMode(QFileDialog::ExistingFiles);
	QStringList list;

	if (importFileDialog.exec())
	{
		list = importFileDialog.selectedFiles(); //get the selected files after click open

		// Loop through all files
		for(int i = 0; i < list.size(); i++)
		{
			// Attempt to import the SVG into pullplan
			SVG::SVG svg;
			PullPlan *newEditorPlan = new PullPlan(PullTemplate::BASE_SQUARE);
			if (SVG::load_svg(list.at(i).toUtf8().constData(), svg, newEditorPlan)) 
			{
				// Test if it is square
				if (svg.page.c[0] == svg.page.c[1]) 
				{
					emit newPullPlan(newEditorPlan);
					pullPlanEditorWidget->update();
				} 
				else 
				{
					// If its not square, give a little error message
					QMessageBox::warning(this, tr("Invalid File"), tr("The SVG file appears to not be square :-("));
				}
			} 
			else 
			{
				// If import fails, give an error message
				QMessageBox::warning(this, tr("Import Failed"), tr("Failed to import SVG file :-("));
			}
		}
	}
}


void MainWindow::getLibraryContents(vector<GlassColor*>* colors, vector<PullPlan*>* plans, vector<Piece*>* pieces)
{
	AsyncColorBarLibraryWidget* cblw;
	for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
	{
		cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
		colors->push_back(cblw->getGlassColor());
	}

	AsyncPullPlanLibraryWidget* pplw;
	for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
	{
		pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
			dynamic_cast<QWidgetItem*>(pullPlanLibraryLayout->itemAt(i))->widget());
		plans->push_back(pplw->getPullPlan());
	}

	AsyncPieceLibraryWidget* plw;
	for (int i = 0; i < pieceLibraryLayout->count(); ++i)
	{
		plw = dynamic_cast<AsyncPieceLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
		pieces->push_back(plw->getPiece());
	}
}


void MainWindow::newFile()
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
				saveAllFile();
				break;
			case QMessageBox::Discard:
				break;
		}
	}

	// reset the system:

	// 1. set editor objects to new (but default) objects
	colorEditorWidget->resetGlassColor();
	pullPlanEditorWidget->resetPlan();
	pieceEditorWidget->resetPiece();

	// 2. delete everything in the library
	clearLibrary();

	// 3. add the three new guys from the editors into the library 
        colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(colorEditorWidget->getGlassColor()));
        pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(pullPlanEditorWidget->getPlan()));
	pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(pieceEditorWidget->getPiece())); 

	// 4. go back to empty view mode
	setViewMode(EMPTY_VIEW_MODE);

	// reset filename
	setSaveFilename("[unsaved]");
	setDirtyBit(false);
}


void MainWindow::openFile()
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
	
	// do the dialog
	QString userSpecifiedFilename = QFileDialog::getOpenFileName(this, tr("Open file..."), QDir::currentPath(), 
		tr("VirtualGlass (*.glass)"));
	if (userSpecifiedFilename.isNull())
		return;

	// try to read in the file
	vector<GlassColor*> colors;
	vector<PullPlan*> plans;
	vector<Piece*> pieces;
	bool success = GlassFileReader::load(userSpecifiedFilename, &colors, &plans, &pieces);

	// if it failed, pop a sad little message box 
	if (!success) 
	{
		QMessageBox msgBox;
		msgBox.setText("The file " + userSpecifiedFilename + " cannot be read.");
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.exec();
		return;
	}		

	// put the read objects into the library	
	clearLibrary();	
	for (unsigned int i = 0; i < colors.size(); ++i)
		colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(colors[i]));
	for (unsigned int i = 0; i < plans.size(); ++i)
		pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(plans[i]));
	for (unsigned int i = 0; i < pieces.size(); ++i)
		pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(pieces[i]));

	// go back to empty mode
	setViewMode(EMPTY_VIEW_MODE);
	
	// set the save file info
	setSaveFilename(userSpecifiedFilename);
	setDirtyBit(false);
}

void MainWindow::addFile()
{
	// do the dialog
	QStringList userSpecifiedFilenames = QFileDialog::getOpenFileNames(this, tr("Open file..."), QDir::currentPath(), 
		tr("VirtualGlass (*.glass)"));
	if (userSpecifiedFilenames.size() == 0) // emptiness iff empty list (hopefully?)
		return;

	// try to read in the files....ALL OF THEM
	vector<GlassColor*> colors;
	vector<PullPlan*> plans;
	vector<Piece*> pieces;
	vector<GlassColor*> partialColors;
	vector<PullPlan*> partialPlans;
	vector<Piece*> partialPieces;
	for (int i = 0; i < userSpecifiedFilenames.size(); ++i)
	{
		partialColors.clear();
		partialPlans.clear();
		partialPieces.clear();
		if (GlassFileReader::load(userSpecifiedFilenames[i], &partialColors, &partialPlans, &partialPieces))
		{
			for (unsigned int j = 0; j < partialColors.size(); ++j)
				colors.push_back(partialColors[i]);
			for (unsigned int j = 0; j < partialPlans.size(); ++j)
				plans.push_back(partialPlans[i]);
			for (unsigned int j = 0; j < partialPieces.size(); ++j)
				pieces.push_back(partialPieces[i]);
		}
		else // yes, we're popping up a dialog for every file that can't be read...don't try 1000 at a time
		{
			QMessageBox msgBox;
			msgBox.setText("The file " + userSpecifiedFilenames[i] + " cannot be read.");
			msgBox.setStandardButtons(QMessageBox::Ok);
			msgBox.exec();
			return;

		}	
	}

	// put the read objects into the library	
	for (unsigned int i = 0; i < colors.size(); ++i)
		colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(colors[i]));
	for (unsigned int i = 0; i < plans.size(); ++i)
		pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(plans[i]));
	for (unsigned int i = 0; i < pieces.size(); ++i)
		pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(pieces[i]));

	// go back to empty mode
	setViewMode(EMPTY_VIEW_MODE);

	// turn *on* dirty bit, because we just added stuff
	setDirtyBit(true);
}

void MainWindow::saveAllFile()
{
	if (saveFilename == "[unsaved]")
		saveAllAsFile();
	else
	{
		// call the actual file-saving code in GlassFileWriter
		vector<GlassColor*> colors;
		vector<PullPlan*> plans;
		vector<Piece*> pieces;
		getLibraryContents(&colors, &plans, &pieces);
		GlassFileWriter::save(saveFilename, colors, plans, pieces);	
		setDirtyBit(false);
	}
}

void MainWindow::saveAllAsFile()
{
	// do the dialog thing to set saveFilename
	QString userSpecifiedFilename = QFileDialog::getSaveFileName(this, tr("Save as..."), QDir::currentPath(), tr("VirtualGlass (*.glass)"));
	if (!userSpecifiedFilename.isNull())
		setSaveFilename(userSpecifiedFilename);
	else
		return;

	vector<GlassColor*> colors;
	vector<PullPlan*> plans;
	vector<Piece*> pieces;
	getLibraryContents(&colors, &plans, &pieces);
	GlassFileWriter::save(saveFilename, colors, plans, pieces);	
	setDirtyBit(false);
}

void MainWindow::saveSelectedAsFile()
{
	// grab the one thing you want to save
	vector<GlassColor*> colors;
	vector<PullPlan*> plans;
	vector<Piece*> pieces;

	switch (editorStack->currentIndex())
	{
		case EMPTY_VIEW_MODE:
			return; // nothing to save
		case COLORBAR_VIEW_MODE:
			colors.push_back(colorEditorWidget->getGlassColor());
			break;
		case PULLPLAN_VIEW_MODE:
			plans.push_back(pullPlanEditorWidget->getPlan());
			break;
		case PIECE_VIEW_MODE:
			pieces.push_back(pieceEditorWidget->getPiece());
			break;
	}
	
	// do the dialog thing to get a one-time filename that you save to
	QString userSpecifiedFilename = QFileDialog::getSaveFileName(this, tr("Save as..."), QDir::currentPath(), tr("VirtualGlass (*.glass)"));
	if (userSpecifiedFilename.isNull())
		return;

	// pretend library has one thing in it		
	GlassFileWriter::save(userSpecifiedFilename, colors, plans, pieces);	

	// this doesn't impact dirty bit or saveFilename at all: it's a special operation that 
	// virtualglass has that lives outside of the usual file-editor relationship, e.g. of a text editor. 
}




