#include <QtGui>
#include <map>
#include <json/json.h>
#include <string>
#include <sstream>
#include <fstream>
#include <QTextStream>
#include <iostream>
#include <stdio.h>
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

MainWindow :: MainWindow()
{
	setWindowTitle(windowTitle());
	centralWidget = new QWidget(this);
	this->setCentralWidget(centralWidget);

	centralLayout = new QHBoxLayout(centralWidget);
	setupLibrary();
	setupEditors();
	setupMenus();
	setupConnections();

	seedEverything();
	setViewMode(EMPTY_VIEW_MODE);
	show();

	emit someDataChanged();
	resetDirtyBits();
	whatToDoLabel->setText("Click a library item at left to edit/view.");
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
		title += " - r" + revision + " built on " + date;
	}
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

	connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));
	connect(importAct, SIGNAL(triggered()), this, SLOT(import()));
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

void MainWindow :: setupPieceEditor()
{
	// Setup data objects - the current plan and library widget for this plan
	pieceEditorWidget = new PieceEditorWidget(editorStack);
	pieceEditorWidget->setMainWindow(this);
	pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(pieceEditorWidget->getPiece()));
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
	AsyncColorBarLibraryWidget* starterLibraryWidget =
		new AsyncColorBarLibraryWidget(new GlassColor(), this);
	colorEditorWidget = new ColorEditorWidget(starterLibraryWidget->getGlassColor(), editorStack);
	colorBarLibraryLayout->addWidget(starterLibraryWidget);
}

void MainWindow :: setupPullPlanEditor()
{
	// Setup data objects - the current plan and library widget for this plan
	pullPlanEditorWidget = new PullPlanEditorWidget(editorStack);
	pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(pullPlanEditorWidget->getPlan()));
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

void MainWindow :: resetDirtyBits()
{
	AsyncColorBarLibraryWidget* cblw;
	for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
	{
		cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
		cblw->getGlassColor()->setDirtyBitColor(false);
	}

	AsyncPullPlanLibraryWidget* pplw;
	for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
	{
		pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
			dynamic_cast<QWidgetItem*>(pullPlanLibraryLayout->itemAt(i))->widget());
		pplw->getPullPlan()->setDirtyBitBool(false);
	}

	AsyncPieceLibraryWidget* plw;
	for (int i = 0; i < pieceLibraryLayout->count(); ++i)
	{
		plw = dynamic_cast<AsyncPieceLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
		plw->getPiece()->setDirtyBitPiece(false);
		plw->getPiece()->pickup->setDirtyBitPick(false);
	}
}

bool MainWindow :: getDirtyBits()
{
	bool dirtyBits = false;

	AsyncPieceLibraryWidget* plw;
	for (int i = 0; i < pieceLibraryLayout->count(); ++i)
	{
		plw = dynamic_cast<AsyncPieceLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
		cout << "dirtyBits piece " << i << " " << dirtyBits;
		cout << endl;
		if(plw->getPiece()->pickup->getDirtyBitPick())
			dirtyBits = true;
		if(plw->getPiece()->getDirtyBitPiece() | dirtyBits)
		{
			dirtyBits = true;
			i = pieceLibraryLayout->count(); //exit loop
		}

		AsyncPullPlanLibraryWidget* pplw;
		for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
		{
			pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
				dynamic_cast<QWidgetItem*>(pullPlanLibraryLayout->itemAt(i))->widget());
					if (plw->getPiece()->hasDependencyOn(pplw->getPullPlan()))
					{
						cout << "dirtyBits pullplan " << i << " " << dirtyBits;
						cout << endl;
						if(pplw->getPullPlan()->getDirtyBitPull() | dirtyBits)
						{
							dirtyBits= true;
							i = pullPlanLibraryLayout->count();
						}
						AsyncColorBarLibraryWidget* cblw;
						for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
						{
							cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(
								dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
							if (pplw->getPullPlan()->hasDependencyOn(cblw->getGlassColor())| plw->getPiece()->hasDependencyOn(cblw->getGlassColor()))
							{
								cout << "dirtyBits color " << i << " " << dirtyBits;
								cout << endl;
								if(cblw->getGlassColor()->getDirtyBitColor())
								{
									dirtyBits = true;
									i = colorBarLibraryLayout->count();
								}
								cout << "dirtyBits color after " << i << " " << dirtyBits;
								cout << endl;
							}
						}
					}
		}
	}
	cout << "dirtyBits end " << dirtyBits;
	cout << endl;
	return dirtyBits;
}

void MainWindow::closeEvent(QCloseEvent *event){
	//maybe save settings to open the programm with closed settings?
	char path[509]; //MS max path 248 chars, max filename 260 chars, plus 1 for terminator
	ifstream readHdl;
	readHdl.open("save");
	readHdl.getline(path,509);
	string strPath;
	strPath.assign(path, strlen(path));
	readHdl.close();

	if(getDirtyBits())
	{
		string message;
		QMessageBox saveMsgBox;
		saveMsgBox.setIconPixmap(QPixmap("virtualglass.png"));
		saveMsgBox.addButton(tr("Don't save"), QMessageBox::YesRole);
		QPushButton *cancelButton = saveMsgBox.addButton(tr("Cancel"), QMessageBox::ActionRole);
		QPushButton *saveAsButton = saveMsgBox.addButton(tr("Save As"), QMessageBox::ActionRole);
		QPushButton	*saveButton = saveMsgBox.addButton(tr("Save"), QMessageBox::ActionRole);

		if(strPath.empty())
		{
			saveAsButton->setText("Save");
			message = "Do you want to save the changes you made?";
			saveButton->hide();
		}
		else
			message = "Do you want to save the changes you made to " + strPath;

		saveMsgBox.setText(message.c_str());
		saveMsgBox.exec();

		if (saveMsgBox.clickedButton() == saveButton)
			save(strPath.c_str());

		if (saveMsgBox.clickedButton() == saveAsButton)
			saveAllAsFile();

		if (saveMsgBox.clickedButton() == cancelButton)
		{
			//ignore close event
			event->ignore();
			return;
		}
	}

	//clear save file
	QFile savePath("save");
	savePath.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream savePathOutput(&savePath);
	savePathOutput << "";
	savePath.close();

	event->accept();
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);
	menu.exec(event->globalPos());
}

void MainWindow::buildCaneTree(PullPlan* plan, vector<PullPlan*>* caneVec, vector<GlassColor*>* colorVec){

	if(std::find(caneVec->begin(), caneVec->end(), plan) != caneVec->end())
		return;

	for(unsigned int i = 0; i < plan->getCasingCount(); i++){
		if (std::find(colorVec->begin(), colorVec->end(), plan->getCasingColor(i)) == colorVec->end())
			colorVec->push_back(plan->getCasingColor(i));
	}
	if(plan->subs.size() == 0){
		if (std::find(caneVec->begin(), caneVec->end(), plan) == caneVec->end())
			caneVec->push_back(plan);
	}
	else{
		for(unsigned int i = 0; i< plan->subs.size(); i++){
			SubpullTemplate subplan = plan->subs.at(i);
			//go down and build tree
			buildCaneTree(subplan.plan, caneVec, colorVec);
			if (std::find(caneVec->begin(), caneVec->end(), subplan.plan) == caneVec->end())
				caneVec->push_back(subplan.plan);
		}
	}
}

void MainWindow::buildCaneMap(vector<PullPlan*>* caneVec, vector<GlassColor*>* colorVec, int selected){
	AsyncPieceLibraryWidget *plw;
	for (int j = 0; j < pieceLibraryLayout->count(); ++j){
		plw = dynamic_cast<AsyncPieceLibraryWidget*>(dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(j))->widget());

		Piece* piece = plw->getPiece();
		//build map the selected piece only
		if(selected==1){
			j=(pieceLibraryLayout->count())+1;
			piece = (pieceEditorWidget->getPiece());
		}

		for(unsigned int i = 0; i < piece->pickup->subs.size(); i++){
			PullPlan* plan = piece->pickup->subs.at(i).plan;

			if (std::find(caneVec->begin(), caneVec->end(), plan) == caneVec->end())
			{
				if (std::find(colorVec->begin(), colorVec->end(), piece->pickup->overlayGlassColor) == colorVec->end())
					colorVec->push_back(piece->pickup->overlayGlassColor);
				if (std::find(colorVec->begin(), colorVec->end(), piece->pickup->casingGlassColor) == colorVec->end())
					colorVec->push_back(piece->pickup->casingGlassColor);
				if (std::find(colorVec->begin(), colorVec->end(), piece->pickup->underlayGlassColor) == colorVec->end())
					colorVec->push_back(piece->pickup->underlayGlassColor);

				buildCaneTree(plan, caneVec, colorVec);
				if (std::find(caneVec->begin(), caneVec->end(), plan) == caneVec->end())
					caneVec->push_back(plan);
			}
		}
	}
}

void MainWindow::writeCane(Json::Value *root, map<PullPlan*, int>* caneMap, map<GlassColor*, int> colorMap, vector<PullPlan*> caneVec){
	AsyncPullPlanLibraryWidget* pplw;
	for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
	{
		pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
		PullPlan* plan=pplw->getPullPlan();
		if(((std::find(caneVec.begin(), caneVec.end(), plan)) != caneVec.end())|(plan==GlobalGlass::circlePlan())|(plan==GlobalGlass::squarePlan())){
			if((plan==GlobalGlass::circlePlan())|(plan==GlobalGlass::squarePlan()))
				(*caneMap)[plan] = 0;
			else
				(*caneMap)[plan] = i+1;
		}
	}

	Json::Value *pullplan_nested = new Json::Value;
	map<PullPlan*, int>::iterator position;
	for(position = caneMap->begin(); position != caneMap->end();++position){
		PullPlan* plan = position->first;
		int i = position->second;
		//check dependency from piece to plan to save only dependent canes
		Json::Value *value5 = new Json::Value;
		*value5 = PullTemplate::enumToString(plan->getTemplateType());
		Json::Value *value7 = new Json::Value;
		*value7 = (plan->getCasingCount());
		Json::Value *value8 = new Json::Value;
		*value8 = (plan->getTwist());

		Json::Value *nested_value = new Json::Value;

		map<GlassColor*, int>::iterator iter;
		for (unsigned int k = 0; k< plan->getCasingCount(); k++){
			Json::Value *nested_value2 = new Json::Value;

			bool color = false;
			for(iter=colorMap.begin(); iter != colorMap.end(); iter++){
				(*nested_value2)["CasingColor"] = colorMap.find(plan->getCasingColor(k))->second;
				color = true;
			}
			if(color==false)
				(*nested_value2)["CasingColor"] = 0;

			(*nested_value2)["CasingShape"] = plan->getCasingShape(k);
			(*nested_value2)["CasingThickness"] = plan->getCasingThickness(k);
			std::stringstream casingSstr;
			casingSstr << k<<"_Casing";
			string casing = casingSstr.str();
			(*nested_value)[casing] = (*nested_value2);
		}
		Json::Value *nested_value2 = new Json::Value;
		for(unsigned int i = 0; i < plan->getParameterCount(); i++){
			TemplateParameter pullTemplPara;
			plan->getParameter(i, &pullTemplPara);
			(*nested_value2)[(pullTemplPara.name)] = pullTemplPara.value;
		}
		if(plan->getParameterCount() != 0)
			(*nested_value)["PullTemplateParameters"] = *nested_value2;

		(*nested_value)["PullTemplate"] = *value5;
		(*nested_value)["Casing count"] = *value7;
		(*nested_value)["Twists"] = *value8;

		std::stringstream *pullplannrSstr = new std::stringstream;
		*pullplannrSstr  <<i<< "_Cane";
		string pullPlanNr = (*pullplannrSstr).str();

		if(plan->subs.size() !=0)
		{
			Json::Value *nested_value3 = new Json::Value;
			for(unsigned int j = 0; j < plan->subs.size(); j++){
				Json::Value *nested_value2 = new Json::Value;
				SubpullTemplate templ = plan->subs.at(j);

				(*nested_value2)["Shape"] = templ.shape;
				(*nested_value2)["Diameter"] = templ.diameter;
				(*nested_value2)["Cane"] = caneMap->find(templ.plan)->second;

				(*nested_value2)["X"] = templ.location[0];
				(*nested_value2)["Y"] = templ.location[1];
				(*nested_value2)["Z"] = templ.location[2];

				std::stringstream *Sstr = new std::stringstream;
					*Sstr <<j << "_Cane";
				string name = Sstr->str();

				(*nested_value3)[name] = (*nested_value2);
			}
			(*nested_value)["SubPullPlans"] = (*nested_value3);
		}
		(*pullplan_nested)[pullPlanNr] = *nested_value;
		i++;
	}
	(*root)["Canes"] = (*pullplan_nested);
}

void MainWindow::writeColor(Json::Value* root, map<GlassColor*, int>* colorMap, vector<GlassColor*> colorVec){
	AsyncColorBarLibraryWidget* cblw;
	Json::Value *color_nested = new Json::Value;
	for (int i = 0; i < colorBarLibraryLayout->count(); ++i){
		cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
		GlassColor* color = (cblw->getGlassColor());

		if((std::find(colorVec.begin(), colorVec.end(), color)) != colorVec.end()){

			(*colorMap)[color] = i+1; //clear ==0

			Json::Value *nested_value = new Json::Value;

			(*nested_value)["R"] = cblw->getGlassColor()->getColor()->r;
			(*nested_value)["G"] = cblw->getGlassColor()->getColor()->g;
			(*nested_value)["B"] = cblw->getGlassColor()->getColor()->b;
			(*nested_value)["Alpha"] = cblw->getGlassColor()->getColor()->a;

			std::stringstream *colorSstr = new std::stringstream;
			*colorSstr  <<i+1<<"_" << (cblw->getGlassColor()->getName())->toStdString();
			string colorName = colorSstr->str();

			(*color_nested)[colorName] = (*nested_value);
		}
	}
	(*root)["Colors"] = (*color_nested);
}

void MainWindow::writePiece(Json::Value* root, map<Piece*, int>* pieceMap, map<PullPlan*, int>* caneMap, map<GlassColor*, int> colorMap, int selected){
	Json::Value *piece_nested = new Json::Value;

	if(selected==0){
		AsyncPieceLibraryWidget *plw;
		for (int i = 0; i < pieceLibraryLayout->count(); ++i){
			plw = dynamic_cast<AsyncPieceLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());

			Piece* piece = plw->getPiece();
			(*pieceMap)[piece] = i;
		}
	}
	else{
		(*pieceMap)[pieceEditorWidget->getPiece()] = 0; //build map with selected piece only
	}

	map<Piece*, int>::iterator position;
	for(position = (*pieceMap).begin(); position != (*pieceMap).end(); ++position){
		Json::Value *nested_value = new Json::Value;
		Piece* piece = position->first;
		int i = position->second;

		(*nested_value)["OverlayGlassColor"] = colorMap.find((*piece).pickup->overlayGlassColor)->second;
		(*nested_value)["UnderlayGlassColor"] = colorMap.find((*piece).pickup->underlayGlassColor)->second;
		(*nested_value)["CasingGlassColor"] = colorMap.find((*piece).pickup->casingGlassColor)->second;

		Json::Value *nested_value3 = new Json::Value;
		Json::Value *nested_value5 = new Json::Value;
		for(unsigned int i = 0; i< piece->pickup->getParameterCount(); i++){
			TemplateParameter pickTemplPara;
			piece->pickup->getParameter(i, &pickTemplPara);
			(*nested_value5)[(pickTemplPara.name)] = pickTemplPara.value;
		}
		(*nested_value3)["SubPickupTemplateParameters"] =  (*nested_value5);

		Json::Value *nested_value4 = new Json::Value;
		for(unsigned int i = 0; i < piece->getParameterCount(); i++){
			TemplateParameter pieceTemplPara;
			piece->getParameter(i,&pieceTemplPara);
			std::stringstream *Sstr = new std::stringstream;
			*Sstr <<i << "_" <<pieceTemplPara.name;
			string name = Sstr->str();
			(*nested_value4)[name] = pieceTemplPara.value;
		}
		(*nested_value)["PieceTemplateParameters"] = (*nested_value4);

		for(unsigned int j = 0; j < piece->pickup->subs.size(); j++){
			Json::Value *nested_value2 = new Json::Value;
			SubpickupTemplate templ = (*piece).pickup->subs.at(j);
			map<PullPlan*, int>::iterator iter;
			(*nested_value2)["Cane"] = 0;
			for(iter = caneMap->begin(); iter != caneMap->end(); iter++){
				if(iter->first==templ.plan)
					(*nested_value2)["Cane"] = iter->second;
			}

			(*nested_value2)["Length"] = templ.length;
			(*nested_value2)["Orientation"] = templ.orientation;
			(*nested_value2)["Shape"] = templ.shape;
			(*nested_value2)["Width"] = templ.width;
			(*nested_value2)["X"] = templ.location[0];
			(*nested_value2)["Y"] = templ.location[1];
			(*nested_value2)["Z"] = templ.location[2];

			std::stringstream *Sstr = new std::stringstream;
			*Sstr <<j << "_Cane";
			string name = Sstr->str();

			(*nested_value3)[name] = (*nested_value2);

			(*nested_value3)["SubPickupTemplate"] =(PickupTemplate::enumToString(piece->pickup->getTemplateType()));
		}
		(*nested_value)["SubPickups"] = (*nested_value3);
		(*nested_value)["PieceTemplate"] = PieceTemplate::enumToString(piece->getTemplateType());
		std::stringstream *Sstr = new std::stringstream;
		*Sstr << i<<"_Piece";
		string name = Sstr->str();
		(*piece_nested)[name]=(*nested_value);
	}
	(*root)["Pieces"] = (*piece_nested);
}

void MainWindow::openColors(Json::Value rootColor, map<GlassColor*, int>* colorMap){
	//rootXXX variables are Json::Values; getMemberNames available
	//vecXXX variables are vectors; operator [] available
	vector<std::string> vecColorMembers = rootColor.getMemberNames(); //vector for colornames
	enum colors{
		R,
		G,
		B,
		alpha
	};

	static std::map<std::string, int> colorMapEnum;
	colorMapEnum["R"] = R;
	colorMapEnum["G"] = G;
	colorMapEnum["B"] = B;
	colorMapEnum["Alpha"] = alpha;

    unsigned int *index = new unsigned int[vecColorMembers.size()];
	unsigned j = 0;
	for (unsigned int i = 0; i < vecColorMembers.size(); i++) //loop over all canes
	{
		string member = vecColorMembers[i];
		unsigned int number =0;
		//fixes "wrong" order (0, 10, 11..., 1, 2 ,.., 9)
		if(member.find("_")!=std::string::npos){
			member.resize(vecColorMembers[i].find("_"));
			number = atoi(member.c_str());
		}

		if(i+1==number)
			index[i]=number-1;
		else
		{
			for (unsigned int l = 0; l < vecColorMembers.size(); l++) //loop over all canes
			{
				string member = vecColorMembers[l];
				if(member.find("_")!=std::string::npos){
					member.resize(vecColorMembers[l].find("_"));
					number = atoi(member.c_str());
				}
				if(i+1==number)
				{
					index[i]=l;
					l=vecColorMembers.size();
				}
			}
		}
	}

	for (unsigned int i = 0; i< vecColorMembers.size(); i++) //loop over all pieces; adds pieces in correct order
	{
		j=index[i];
		Json::Value rootColorValue = rootColor[vecColorMembers.at(j)];
		vector<std::string> vecColorValueMembers = rootColorValue.getMemberNames(); //vector for RGBalpha

		Color *color = new Color;
		GlassColor *glasscolor = new GlassColor;

		if(vecColorValueMembers.size()==4){
			for(int k=0; k<4; k++){
				switch(colorMapEnum[vecColorValueMembers.at(k)]){
				case R : ((*color).c)[0] = rootColorValue["R"].asFloat(); break;
				case G : ((*color).c)[1] = rootColorValue["G"].asFloat(); break;
				case B : ((*color).c)[2] = rootColorValue["B"].asFloat(); break;
				case alpha : ((*color).c)[3] = rootColorValue["Alpha"].asFloat(); break;
				}
				(*glasscolor).setColor(*color);
			}
			string colorNumberSt = (vecColorMembers.at(j));
			colorNumberSt.resize((vecColorMembers.at(j)).find("_")); //extracts number from string
			int colorNumberInt = atoi(colorNumberSt.c_str());
			(*colorMap)[glasscolor] = colorNumberInt;

			string colorName = (vecColorMembers.at(j)).substr ((vecColorMembers.at(j)).find("_")+1);
			(*glasscolor).setName(QString::fromStdString(colorName));
			AsyncColorBarLibraryWidget *w =new AsyncColorBarLibraryWidget(glasscolor, this);
			colorBarLibraryLayout->addWidget(w);
			colorEditorWidget->setGlassColor(glasscolor);
			colorBarLibraryLayout->update();
			// Trigger GUI updates
			w->updatePixmaps();
			emit someDataChanged();
		}
		else
			cout << "error in color " << vecColorValueMembers[j];
	}

    delete index;
}

void MainWindow::openCanes(Json::Value rootCane, map<PullPlan*, int>* caneMap, map<GlassColor*, int>* colorMap){
	vector<std::string> vecCaneMembers = rootCane.getMemberNames(); //vector for canes
	map<GlassColor*, int>::iterator iter;

	enum caneKeywords{
		Subpullplans,
		casingcount,
		PullTemplate,
		PullTemplateParameters,
		twists,
		Column,
		Count, //Column count
		Row, //Row count
		Radial, //Radial count
		CasingColor,
		CasingShape,
		CasingThickness,
		Undefined,
		BaseCircle,
		BaseSquare,
		HorizontalLineCircle,
		HorizontalLineSquare,
		Tripod,
		Cross,
		SquareofCircles,
		SquareofSquares,
		SurroundingCircle,
		SurroundingSquare,
		Custom
	};

	static std::map<std::string, int> caneMapEnum;
	caneMapEnum["SubPullPlans"] = Subpullplans;
	caneMapEnum["PullTemplate"] = PullTemplate;
	caneMapEnum["PullTemplateParameters"] = PullTemplateParameters;
	caneMapEnum["Casing count"] = casingcount;
	caneMapEnum["Twists"] = twists;
	caneMapEnum["Column"] = Column;
	caneMapEnum["Count"] = Count;
	caneMapEnum["Row"] = Row;
	caneMapEnum["Radial"] = Radial;
	caneMapEnum["CasingColor"] = CasingColor;
	caneMapEnum["CasingShape"] = CasingShape;
	caneMapEnum["CasingThickness"] = CasingThickness;
	caneMapEnum["Undefined"] = Undefined;
	caneMapEnum["Base Circle"] = BaseCircle;
	caneMapEnum["Base Square"] = BaseSquare;
	caneMapEnum["Horizontal Line Circle"] = HorizontalLineCircle;
	caneMapEnum["Horizontal Line Square"] = HorizontalLineSquare;
	caneMapEnum["Tripod"] = Tripod;
	caneMapEnum["Cross"] = Cross;
	caneMapEnum["Square of Circles"] = SquareofCircles;
	caneMapEnum["Square of Squares"] = SquareofSquares;
	caneMapEnum["Surrounding Circle"] = SurroundingCircle;
	caneMapEnum["Surrounding Square"] = SurroundingSquare;
	caneMapEnum["Custom"] = Custom;

    unsigned int *index = new unsigned int[vecCaneMembers.size()];
	unsigned k = 0;
	for (unsigned int i = 0; i < vecCaneMembers.size(); i++) //loop over all canes
	{
		string member = vecCaneMembers[i];
		unsigned int number =0;
		//fixes "wrong" order (0, 10, 11..., 1, 2 ,.., 9)
		if(member.find("_")!=std::string::npos){
			member.resize(vecCaneMembers[i].find("_"));
			number = atoi(member.c_str());
		}

		if(i+1==number)
			index[i]=number-1;
		else
		{
			for (unsigned int l = 0; l < vecCaneMembers.size(); l++) //loop over all canes
			{
				string member = vecCaneMembers[l];
				if(member.find("_")!=std::string::npos){
					member.resize(vecCaneMembers[l].find("_"));
					number = atoi(member.c_str());
				}
				if(i+1==number)
				{
					index[i]=l;
					l=vecCaneMembers.size();
				}
			}
		}
	}

	for (unsigned int i = 0; i < vecCaneMembers.size(); i++) //loop over all pieces; adds pieces in correct order
	{
		k=index[i];
		cout << "k " << vecCaneMembers.at(k);
		cout << endl;

		Json::Value rootCaneValue = rootCane[vecCaneMembers.at(k)];
		vector<std::string> vecCaneValueMembers = rootCaneValue.getMemberNames(); //vector for CaneValues
		PullPlan *plan = new PullPlan(PullTemplate::BASE_CIRCLE);
		AsyncPullPlanLibraryWidget *p = new AsyncPullPlanLibraryWidget(plan);
		string caneNumberSt = (vecCaneMembers.at(k));
		caneNumberSt.resize((vecCaneMembers.at(k)).find("_")); //extracts number from string
		int caneNumberInt = atoi(caneNumberSt.c_str());
		for(unsigned int l = 0; l < vecCaneValueMembers.size(); l++){
			Json::Value rootCaneCasing;
			switch(caneMapEnum[vecCaneValueMembers.at(l)]){
			case casingcount:
			{
				rootCaneCasing = rootCaneValue["0_Casing"];
				if(rootCaneCasing["CasingShape"].asBool())
					plan->setTemplateType(PullTemplate::BASE_SQUARE);
				else
					plan->setTemplateType(PullTemplate::BASE_CIRCLE);

				for(unsigned int i = 0; i < rootCaneValue["Casing count"].asUInt()-2; i++){
					std::stringstream *sstr = new std::stringstream;
					*sstr << i+2<<"_Casing";
					string casing = sstr->str();
					Json::Value rootCaneCasing = rootCaneValue[casing];
					//if(i==plan->getCasingCount()-1){
						if(rootCaneCasing["CasingShape"].asBool())
							plan->addCasing(SQUARE_SHAPE);
						else
							plan->addCasing(CIRCLE_SHAPE);
					//}
				}
				for(int i = 0; i < rootCaneValue["Casing count"].asInt(); i++){
					std::stringstream *sstr = new std::stringstream;
					*sstr << i<<"_Casing";
					string casing = sstr->str();
					for(iter = colorMap->begin(); iter != colorMap->end(); iter++){
						if(iter->second == rootCaneValue[casing]["CasingColor"].asInt())
							plan->setCasingColor(iter->first,i);
					}
				}
				int i=plan->getCasingCount();
				while(i>0){
					std::stringstream *sstr = new std::stringstream;
					*sstr << i-1<<"_Casing";
					string casing = sstr->str();
					Json::Value rootCaneCasing = rootCaneValue[casing];
					plan->setCasingThickness(rootCaneCasing["CasingThickness"].asFloat(),i-1);
					i--;
				}
			}
			break;
			case PullTemplate:
			{
			switch(caneMapEnum[rootCaneValue["PullTemplate"].asString()]){
			case BaseCircle:
				plan->setTemplateType(PullTemplate::BASE_CIRCLE);
				break;
			case BaseSquare:
				plan->setTemplateType(PullTemplate::BASE_SQUARE);
				break;
			case HorizontalLineCircle:
				plan->setTemplateType(PullTemplate::HORIZONTAL_LINE_CIRCLE);
				break;
			case HorizontalLineSquare:
				plan->setTemplateType(PullTemplate::HORIZONTAL_LINE_SQUARE);
				break;
			case Tripod:
				plan->setTemplateType(PullTemplate::TRIPOD);
				break;
			case Cross:
				plan->setTemplateType(PullTemplate::CROSS);
				break;
			case SquareofCircles:
				plan->setTemplateType(PullTemplate::SQUARE_OF_CIRCLES);
				break;
			case SquareofSquares:
				plan->setTemplateType(PullTemplate::SQUARE_OF_SQUARES);
				break;
			case SurroundingCircle:
				plan->setTemplateType(PullTemplate::SURROUNDING_CIRCLE);
				break;
			case SurroundingSquare:
				plan->setTemplateType(PullTemplate::SURROUNDING_SQUARE);
				break;
			case Custom:
				plan->setTemplateType(PullTemplate::CUSTOM);
				break;
		}
		break;
			}
			case twists:
				plan->setTwist(rootCaneValue["Twists"].asInt());
				break;
			case PullTemplateParameters:
				if(rootCaneValue.isMember("PullTemplateParameters"))
				{
					vector<std::string> pullTemplateParameterValues = rootCaneValue["PullTemplateParameters"].getMemberNames();
					plan->setParameter(0,rootCaneValue["PullTemplateParameters"][pullTemplateParameterValues[0]].asInt()); //first parameter!
					break;
				}
			}
		}
		pullPlanLibraryLayout->addWidget(p);
		pullPlanLibraryLayout->update();
		emit someDataChanged();
		(*caneMap)[plan] = caneNumberInt;
	}
	//fill subs
	map<PullPlan*, int>::iterator pullIter;
	map<PullPlan*, int>::iterator subpullIter;
	//unsigned int j = 0;
	for(pullIter = caneMap->begin(); pullIter != caneMap->end(); pullIter++){
		PullPlan *plan = new PullPlan(PullTemplate::BASE_CIRCLE);
		plan = pullIter->first;
		if(plan->subs.size()>1){
			AsyncPullPlanLibraryWidget *p = new AsyncPullPlanLibraryWidget(plan);
			for(unsigned int j = 0; j<vecCaneMembers.size(); j++){
				string numberSt = vecCaneMembers.at(j);
				int numberInt = 0;
				if(numberSt.find("_") != std::string::npos){
					numberSt.resize(numberSt.find("_"));
					numberInt = atoi(numberSt.c_str());
				}
				if(numberInt==pullIter->second){
					Json::Value rootCaneValue = rootCane[vecCaneMembers.at(j)];
					if(rootCaneValue["SubPullPlans"] != rootCaneValue["NULL"]){
						Json::Value rootCaneSubpull = rootCaneValue["SubPullPlans"];
						vector<std::string> vecCaneSubpullMembers = rootCaneSubpull.getMemberNames();

						for (unsigned int i = 0; i < vecCaneSubpullMembers.size(); i++){
							Json::Value rootSubcane = rootCaneSubpull[vecCaneSubpullMembers.at(i)];
							string member = vecCaneSubpullMembers.at(i);
							int number =0;
							if(member.find("_") != std::string::npos){
								member.resize((vecCaneSubpullMembers.at(i)).find("_"));
								number=atoi(member.c_str());
							}

							for(subpullIter = caneMap->begin(); subpullIter != caneMap->end(); subpullIter++) {
								if (subpullIter->second == rootSubcane["Cane"].asInt())
								{
									PullPlan *subplan = new PullPlan(PullTemplate::BASE_CIRCLE);
									subplan = subpullIter->first;
									GeometricShape shape;
									if (rootSubcane["Shape"].asInt() == 0)
										shape = CIRCLE_SHAPE;
									else
										shape = SQUARE_SHAPE;
									Point location;
									location.x = rootSubcane["X"].asFloat();
									location.y = rootSubcane["Y"].asFloat();
									location.z = rootSubcane["Z"].asFloat();

									SubpullTemplate *sub = new SubpullTemplate(subplan, shape, location, rootSubcane["Diameter"].asFloat());
									plan->subs[number] =  *sub;
								}
							}
						}
					}
				}
				p->updatePixmaps();
			}
		}
	}
	pullPlanLibraryLayout->update();
	emit someDataChanged();
    delete index;
}

void MainWindow::openPieces(Json::Value root, map<PullPlan*, int>* caneMap, map<GlassColor*, int>* colorMap)
{
	std::vector<std::string> vecPieceMembers = root.getMemberNames(); //vec with pieces

	enum piece{
		subPickupTemplateParameters,
		subPickupTemplate,
		subPickups,
		overlayGlassColor,
		underlayGlassColor,
		casingGlassColor,
		Tumbler,
		Vase,
		Bowl,
		Pot,
		Pickup,
		Wavy_Plate,
		vertical,
		reticello,
		murrinecolumn,
		verticalsandhorizontals,
		verthorizontalvert,
		verticalwithlipwrap,
		murrinerow,
		murrine,
		cane,
		length,
		orientation,
		shape,
		width,
		x,
		y,
		z,
		row,
		thickness,
		column,
		lipwidth,
		bodywidth,
		twists
	};

	static std::map<std::string, int> mapEnum;
	mapEnum["OverlayGlassColor"] = overlayGlassColor;
	mapEnum["UnderlayGlassColor"] = underlayGlassColor;
	mapEnum["CasingGlassColor"] = casingGlassColor;
	mapEnum["SubPickupTemplate"] = subPickupTemplate;
	mapEnum["SubPickupTemplateParameters"] = subPickupTemplateParameters;
	mapEnum["SubPickups"] = subPickups;
	mapEnum["Tumbler"] = Tumbler;
	mapEnum["Vase"] = Vase;
	mapEnum["Bowl"] = Bowl;
	mapEnum["Pot"] = Pot;
	mapEnum["Pickup"] = Pickup;
	mapEnum["Wavy Plate"] = Wavy_Plate;
	mapEnum["Vertical"] = vertical;
	mapEnum["Reticello Vertical Horizontal"] = reticello;
	mapEnum["Murrine Column"] = murrinecolumn;
	mapEnum["Verticals and Horizontals"] = verticalsandhorizontals;
	mapEnum["Vertical Horizontal Vertical"] = verthorizontalvert;
	mapEnum["Vertical With Lip Wrap"] = verticalwithlipwrap;
	mapEnum["Murrine Row"] = murrinerow;
	mapEnum["Murrine"] = murrine;
	mapEnum["Cane"] = cane;
	mapEnum["Length"] = length;
	mapEnum["Orientation"] = orientation;
	mapEnum["Shape"] = shape;
	mapEnum["Width"] = width;
	mapEnum["X"] = x;
	mapEnum["Y"] = y;
	mapEnum["Z"] = z;
	mapEnum["Row/Column count"] = row;
	mapEnum["Thickness"] = thickness;
	mapEnum["Column count"] = column;
	mapEnum["Lip width"] = lipwidth;
	mapEnum["Body width"] = bodywidth;
	mapEnum["Twists"] = twists;

    unsigned int *index = new unsigned int[vecPieceMembers.size()];
	unsigned k = 0;
	for (unsigned int i = 0; i < vecPieceMembers.size(); i++) //loop over all pieces
	{
		string member = vecPieceMembers[i];
		unsigned int number =0;
		//fixes "wrong" order (0, 10, 11..., 1, 2 ,.., 9)
		if(member.find("_")!=std::string::npos){
			member.resize(vecPieceMembers[i].find("_"));
			number = atoi(member.c_str());
		}

		if(i==number)
		{
			index[i]=number;
			cout << "bla";
			cout << endl;
		}
		else
		{
			for (unsigned int l = 0; l < vecPieceMembers.size(); l++) //loop over all pieces
			{
				string member = vecPieceMembers[l];
				if(member.find("_")!=std::string::npos){
					member.resize(vecPieceMembers[l].find("_"));
					number = atoi(member.c_str());
				}
				if(i==number)
				{
					index[i]=l;
					l=vecPieceMembers.size();
				}
			}
		}
	}

	for (unsigned int i = 0; i < vecPieceMembers.size(); i++) //loop over all pieces; adds pieces in correct order
	{
		k=index[i];

		Json::Value rootPieceValues = root[vecPieceMembers[k]];
        PieceTemplate::Type ptt = PieceTemplate::TUMBLER;
		switch(mapEnum[rootPieceValues["PieceTemplate"].asString()])
		{
			case 	Tumbler :
				ptt = PieceTemplate::TUMBLER;
				break;
			case Vase :
				ptt = PieceTemplate::VASE;
				break;
			case Bowl :
				ptt = PieceTemplate::BOWL;
				break;
			case Pot :
				ptt = PieceTemplate::POT;
				break;
			case Pickup :
				ptt = PieceTemplate::PICKUP;
				break;
			case Wavy_Plate :
				ptt = PieceTemplate::WAVY_PLATE;
				break;
		}

		Piece *piece = new Piece(ptt);
		map<GlassColor*,int>::iterator iter;
		for(iter = colorMap->begin(); iter != colorMap->end(); iter++){
			if(rootPieceValues["CasingGlassColor"].asInt()==iter->second)
				piece->pickup->casingGlassColor = iter->first;
			if(rootPieceValues["OverlayGlassColor"].asInt()==iter->second)
				piece->pickup->overlayGlassColor = iter->first;
			if(rootPieceValues["UnderlayGlassColor"].asInt()==iter->second)
				piece->pickup->underlayGlassColor = iter->first;
		}
		if(rootPieceValues.isMember("PieceTemplateParameters"))
		{
			std::vector<std::string> vecPieceTemplParameters = rootPieceValues["PieceTemplateParameters"].getMemberNames();
			for(unsigned int i=0; i< vecPieceTemplParameters.size(); i++){
				if((vecPieceTemplParameters[i].find("_") != std::string::npos)&&(rootPieceValues["PieceTemplateParameters"].isMember(vecPieceTemplParameters[i])))
						piece->setParameter(i, rootPieceValues["PieceTemplateParameters"][vecPieceTemplParameters[i]].asInt());
			}
		}

		if(rootPieceValues["SubPickups"]["SubPickupTemplate"]!=rootPieceValues["NULL"])
		{
			switch(mapEnum[rootPieceValues["SubPickups"]["SubPickupTemplate"].asString()])
			{
				case vertical:
				piece->pickup->setTemplateType(PickupTemplate::VERTICAL);
					break;
				case reticello:
					piece->pickup->setTemplateType(PickupTemplate::RETICELLO_VERTICAL_HORIZONTAL);
					break;
				case murrinecolumn:
					piece->pickup->setTemplateType(PickupTemplate::MURRINE_COLUMN);
					break;
				case verticalsandhorizontals:
					piece->pickup->setTemplateType(PickupTemplate::VERTICALS_AND_HORIZONTALS);
					break;
				case verthorizontalvert:
					piece->pickup->setTemplateType(PickupTemplate::VERTICAL_HORIZONTAL_VERTICAL);
					break;
				case verticalwithlipwrap:
					piece->pickup->setTemplateType(PickupTemplate::VERTICAL_WITH_LIP_WRAP);
					break;
				case murrinerow:
					piece->pickup->setTemplateType(PickupTemplate::MURRINE_ROW);
					break;
				case murrine:
					piece->pickup->setTemplateType(PickupTemplate::MURRINE);
					break;
				}
		}
		if(rootPieceValues["SubPickups"].isMember("SubPickupTemplateParameters"))  //in the moment not necessary. safety first!
		{
			std::vector<std::string> vecSubPickupPara = rootPieceValues["SubPickups"]["SubPickupTemplateParameters"].getMemberNames();
			if(vecSubPickupPara.size()<3)
			{
				if((rootPieceValues["SubPickups"]["SubPickupTemplateParameters"].isMember("Column count"))&&(rootPieceValues["SubPickups"]["SubPickupTemplateParameters"]["Column count"]!=rootPieceValues["NULL"]))
					piece->pickup->setParameter(0, rootPieceValues["SubPickups"]["SubPickupTemplateParameters"]["Column count"].asInt());

				if((rootPieceValues["SubPickups"]["SubPickupTemplateParameters"].isMember("Row/Column count"))&&(rootPieceValues["SubPickups"]["SubPickupTemplateParameters"]["Row/Column count"]!=rootPieceValues["NULL"]))
					piece->pickup->setParameter(0, rootPieceValues["SubPickups"]["SubPickupTemplateParameters"]["Row/Column count"].asInt());

				if(rootPieceValues["SubPickups"]["SubPickupTemplateParameters"].isMember("Thickness")&&(rootPieceValues["SubPickups"]["SubPickupTemplateParameters"]["Thickness"]!=rootPieceValues["NULL"]))
					piece->pickup->setParameter(1, rootPieceValues["SubPickups"]["SubPickupTemplateParameters"]["Thickness"].asInt());
			}
		}

		std::vector<std::string> vecSubPickups = rootPieceValues["SubPickups"].getMemberNames();

		for(unsigned int j = 0; j < vecSubPickups.size()-2; j++)
		{

			string member = vecSubPickups[j];
			int number =0;
			//fixes "wrong" order (0, 10, 11..., 1, 2 ,.., 9)
			if(member.find("_")!=std::string::npos){
				member.resize(vecSubPickups[j].find("_"));
				number = atoi(member.c_str());
			}

			std::ostringstream help;
			help << number << "_Cane";
			std::string var = help.str();

			if(rootPieceValues["SubPickups"].isMember(var))
			{
				std::vector<std::string> vecSubPickupCanes = rootPieceValues["SubPickups"][vecSubPickups[number]].getMemberNames();
				Json::Value rootSubPickupCanes = rootPieceValues["SubPickups"][vecSubPickups[number]];

				if(vecSubPickupCanes.size()==8)
				{
					map<PullPlan*, int>::iterator iter;
					PullPlan* plan = new PullPlan(PullTemplate::BASE_CIRCLE);
					for(iter = caneMap->begin();iter != caneMap->end();iter++){
						if(iter->second==(rootSubPickupCanes["Cane"].asInt()))
						{
							plan = iter->first;
						}
					}
					Point location;
					location[0] = rootSubPickupCanes["X"].asFloat();
					location[1] = rootSubPickupCanes["Y"].asFloat();
					location[2] = rootSubPickupCanes["Z"].asFloat();
					GeometricShape shape;
					if(rootSubPickupCanes["Shape"].asInt()==0){
						shape = CIRCLE_SHAPE;
					} else
					{
						shape = SQUARE_SHAPE;
					}

					SubpickupTemplate *pick = new SubpickupTemplate(plan, location, rootSubPickupCanes["Orientation"].asInt(),
						rootSubPickupCanes["Length"].asFloat(), rootSubPickupCanes["Width"].asFloat(), shape);
					piece->pickup->subs.at(number) = *pick;
					pullPlanLibraryLayout->update();
					emit someDataChanged();
					this->updateEverything();
				}
			} else
			{
				cout << "Not enough SubPickups available!" << endl;
			}
		}

			AsyncPieceLibraryWidget *w = new AsyncPieceLibraryWidget(piece, this);
			pieceLibraryLayout->addWidget(w);
			pieceLibraryLayout->update();
			w->updatePixmap();
			emit someDataChanged();
	}
	//repaint pullplan library
	AsyncPullPlanLibraryWidget* pplw;
	for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
	{
		pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
		pplw->updatePixmaps();
	}

    delete index;
}

void MainWindow::open(QStringList list, bool merge){
	for(int i = 0; i < list.size(); i++){
		QString filename = list.at(i);
		if((filename.toStdString())!=""&&list.size()>1){
			QFile savePath("save");
			savePath.open(QIODevice::WriteOnly | QIODevice::Text);
			QTextStream savePathOutput(&savePath);
			savePathOutput << filename << "\n";
			savePath.close();
		}

		QFile openFile(filename);
		openFile.open(QIODevice::ReadOnly | QIODevice::Text);
		QTextStream fileInput(&openFile);
		QString QStr = fileInput.readAll();
		std::string str = QStr.toStdString();
		Json::Value root;
		Json::Reader reader;

		bool parsedSuccess = reader.parse(str,root,false);
		map<GlassColor*, int> colorMap;
		map<PullPlan*, int> caneMap;

		colorMap[new GlassColor] = 0;

		//ready to use if necessary
		//if(((root["Build information"]["Number"]).asString())<"700")
		//	cout << "we do not support your build number " << (root["Build information"]["Number"]).asString() << " any more!" << endl;

		if(!parsedSuccess){
            cout<<"Failed to parse JSON"<<endl<<reader.getFormatedErrorMessages()<<endl; //debugging
		}

		if( root.size() != 4){
			cout << "error in file";
		}
		else{
			if(merge==false&&root["Pieces"].size()!=0)
			{
				if(i==0)
					newFile();
				openColors(root["Colors"], &colorMap);
				openCanes(root["Canes"], &caneMap, &colorMap);
				openPieces(root["Pieces"], &caneMap, &colorMap);
				if(i==0)
					deleteStandardLibraryElements();
			}
			else
			{
				if(merge==true&&root["Pieces"].size()!=0)
				{
					openColors(root["Colors"], &colorMap);
					openCanes(root["Canes"], &caneMap, &colorMap);
					openPieces(root["Pieces"], &caneMap, &colorMap);
				}
			}
		}
	}
}

void MainWindow::deleteStandardLibraryElements(){
	QLayoutItem* w;
		//colors
	if(colorBarLibraryLayout->count()>=1)
	{
		w = colorBarLibraryLayout->takeAt(0);
		delete w->widget();
		delete w;
	}

	//canes
	if(pullPlanLibraryLayout->count()>=1)
	{
		w = pullPlanLibraryLayout->takeAt(0);
		delete w->widget();
		delete w;
	}

	//pieces
	if(pieceLibraryLayout->count()>=1)
	{
		w = pieceLibraryLayout->takeAt(0);
		delete w->widget();
		delete w;
	}

	emit someDataChanged();
}

void MainWindow::newFile(){

	QFile savePath("save");
	savePath.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream savePathOutput(&savePath);
	savePathOutput << "\n";
	savePath.close();
	QLayoutItem* w;

	//colors

	//AsyncColorBarLibraryWidget *cb =new AsyncColorBarLibraryWidget(new GlassColor, this);
	colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(new GlassColor, this));
	colorBarLibraryLayout->update();

	int i;

	if(colorBarLibraryLayout->count()>1) //do not delete an empty library (except of your new element)
	{
		for (i=0; i < colorBarLibraryLayout->count();)
		{
			w = colorBarLibraryLayout->takeAt(i);
			delete w->widget();
			delete w;
			if(colorBarLibraryLayout->count()<=1) //deleting library except of your new element
				i++;
		}
	}

	colorBarLibraryLayout->update();
	emit someDataChanged();

	//pullplan

	AsyncPullPlanLibraryWidget *pplw =new AsyncPullPlanLibraryWidget(new PullPlan(PullTemplate::BASE_CIRCLE),this);
	pullPlanLibraryLayout->addWidget(pplw);
	pullPlanLibraryLayout->update();

	if(pullPlanLibraryLayout->count()>1) //do not delete an empty library (except of your new element)
	{
		for (i=0; i< pullPlanLibraryLayout->count();)
		{
			w=pullPlanLibraryLayout->takeAt(i);
			delete w->widget();
			delete w;
			if(pullPlanLibraryLayout->count()<=1) //clears library except of your new element
				i++;
		}
	}
	pullPlanLibraryLayout->update();
	emit someDataChanged();


	//pieces

	AsyncPieceLibraryWidget *plw = new AsyncPieceLibraryWidget(new Piece(PieceTemplate::TUMBLER));
	pieceLibraryLayout->addWidget(plw);
	pieceLibraryLayout->update();

	for (i=0; i < pieceLibraryLayout->count();)
	{
		w=pieceLibraryLayout->takeAt(i);
		delete w->widget();
		delete w;
		if(pieceLibraryLayout->count()<=1) //clears library except of your new element
			i++;
	}
	pieceLibraryLayout->update();
	plw->updatePixmap();
	unhighlightAllLibraryWidgets();
	setViewMode(EMPTY_VIEW_MODE);
	emit someDataChanged();
}

void MainWindow::import(){
    QFileDialog importFileDialog(this);
	importFileDialog.setOption(QFileDialog::DontUseNativeDialog);
	importFileDialog.setWindowTitle(tr("Open your SCG cane crosssection file"));
	importFileDialog.setNameFilter(tr("Scalable Vector Graphics (*.svg)")); //avoid open non .svg files
	importFileDialog.setFileMode(QFileDialog::ExistingFiles);
	QStringList list;

    if (importFileDialog.exec()){
		list = importFileDialog.selectedFiles(); //get the selected files after click open

        // Loop through all files
		for(int i = 0; i < list.size(); i++){

            // Attempt to import the SVG into pullplan
            SVG::SVG svg;
            PullPlan *newEditorPlan = new PullPlan(PullTemplate::BASE_SQUARE);
            if (SVG::load_svg(list.at(i).toUtf8().constData(), svg, newEditorPlan) ) {

                // Test if it is square
                if (svg.page.c[0]==svg.page.c[1]) {

                    emit newPullPlan(newEditorPlan);
                    pullPlanEditorWidget->update();
                } else {
                    // If its not square, give a little error message
                    QMessageBox::warning ( this, tr("Invalid File"), tr("The SVG file appears to not be square :-("));
                }
            } else {
                // If import fails, give an error message
                QMessageBox::warning ( this, tr("Import Failed"), tr("Failed to import SVG file :-("));
            }
		}
	}
}

void MainWindow::save(QString filename){
	if(!filename.isEmpty())
		resetDirtyBits();
	QFile saveFile(filename);
	saveFile.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream fileOutput(&saveFile);
	//read version and date from version.txt; write this into json file (root0)
	//first line; versionNo and date
	ifstream readHdl;
	//date has always 12 characters
	char date[11];
	char versionNo[4];

	readHdl.open(":/version.txt");
	readHdl.getline(versionNo,4,'\n');
	readHdl.getline(date,11,'\n');
	readHdl.close();

	Json::Value root;
	Json::Value nested_value;
	nested_value["Number"] = versionNo;
	nested_value["Date"] = date;
	root["Build information"] = nested_value;
	map<Piece*,int> pieceMap;
	map<PullPlan*,int> caneMap;
	map<GlassColor*, int> colorMap;
	vector<PullPlan*> caneVec;
	vector<GlassColor*> colorVec;

	colorMap[GlobalGlass::color()] = 0;

	buildCaneMap(&caneVec, &colorVec, 0);
	writeColor(&root, &colorMap, colorVec);
	writeCane(&root, &caneMap, colorMap, caneVec);
	writePiece(&root, &pieceMap, &caneMap, colorMap, 0);

	fileOutput << writeJson(root);
	saveFile.close();
}

void MainWindow::saveAs(QString filename){
	if(!filename.isEmpty())
		resetDirtyBits();
	QFile saveFile(filename);
	saveFile.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream fileOutput(&saveFile);
	//read version and date from version.txt; write this into json file (root0)
	//first line; versionNo and date
	ifstream readHdl;
	//date has always 12 characters
	char date[11];
	char versionNo[4];

	readHdl.open(":/version.txt");
	readHdl.getline(versionNo,4,'\n');
	readHdl.getline(date,11,'\n');
	readHdl.close();

	Json::Value root;
	Json::Value nested_value;
	nested_value["Number"] = versionNo;
	nested_value["Date"] = date;
	root["Build information"] = nested_value;

	map<Piece*,int> pieceMap;
	map<PullPlan*,int> caneMap;
	map<GlassColor*, int> colorMap;
	vector<PullPlan*> caneVec;
	vector<GlassColor*> colorVec;

	colorMap[GlobalGlass::color()] = 0;

	buildCaneMap(&caneVec, &colorVec, 1);
	writeColor(&root, &colorMap, colorVec);
	writeCane(&root, &caneMap, colorMap, caneVec);
	writePiece(&root, &pieceMap, &caneMap, colorMap, 1);

	fileOutput << writeJson(root);
	saveFile.close();
}

void MainWindow::saveAllFile(){
	char path[509]; //MS max path 248 chars, max filename 260 chars, plus 1 forterminator

	ifstream readHdl;

	readHdl.open("save");
	readHdl.getline(path,509);
	string strPath;
	strPath.assign(path, strlen(path));
	readHdl.close();
	QString filename;
	if(strPath.empty()){
		filename = QFileDialog::getSaveFileName(this, tr("Save your glass piece"), QDir::currentPath(), tr("VirtualGlass (*.glass)") );
				//improve: prevent character set error in filename
				//improve: empty file name -> "no savefile choosen"
		if(!(filename.toStdString().empty())){
			QFile savePath("save");
			savePath.open(QIODevice::WriteOnly | QIODevice::Text);
			QTextStream savePathOutput(&savePath);
			savePathOutput << filename << "\n";
			savePath.close();
		}
	}
	else{
		filename = strPath.c_str();
	}
	save(filename);
}

/*void MainWindow::saveSelectedFile(){

	char path[509]; //MS max path 248 chars, max filename 260 chars, plus 1 forterminator
	ifstream readHdl;

	readHdl.open("save");
	readHdl.getline(path,509, '\n');
	string strPath;
	strPath.assign(path, strlen(path));
	readHdl.close();
	QString filename;
	if(strPath==""){
		filename = QFileDialog::getSaveFileName(this, tr("Save your glass piece"), QDir::currentPath(), tr("VirtualGlass (*.glass)") );
				//improve: prevent character set error in filename
				//improve: empty file name -> "no savefile choosen"
		if((filename.toStdString())!=""){
			QFile savePath("save");
			savePath.open(QIODevice::WriteOnly | QIODevice::Text);
			QTextStream savePathOutput(&savePath);
			savePathOutput << filename << "\n";
			savePath.close();
		}
	}
	else{
		filename = strPath.c_str();
	}
	saveAs(filename);
}*/

void MainWindow::saveSelectedAsFile(){
	QString filename = QFileDialog::getSaveFileName(this, tr("Save your glass piece"), QDir::currentPath(), tr("VirtualGlass (*.glass)") );
	if(filename.toStdString()!="")
		saveAs(filename);
}

void MainWindow::saveAllAsFile(){
	//save file dialog
	QString filename = QFileDialog::getSaveFileName(this, tr("Save your glass piece"), QDir::currentPath(), tr("VirtualGlass (*.glass)") );
	//improve: prevent character set error in filename
	//improve: empty file name -> "no savefile choosen"
	if(filename.toStdString()!="")
		save(filename);
}

void MainWindow::setMerge(bool var){
	if(var==true)
		merge =true;
	else
		merge=false;
}

bool MainWindow::getMerge(){
	return merge;
}

void MainWindow::castMergeButton(QWidget* w)
{
    QFileDialog *openFileDialog = dynamic_cast<QFileDialog *>(w);
	Q_ASSERT( openFileDialog );
	QStringList list = openFileDialog->selectedFiles();
	setMerge(true);
	open(list, true);
	openFileDialog->close();
}

void MainWindow::openFile(){

	setMerge(false);
	QFileDialog openFileDialog(this);
	openFileDialog.setOption(QFileDialog::DontUseNativeDialog);
	openFileDialog.setWindowTitle(tr("Open your VirtualGlass file"));
	openFileDialog.setNameFilter(tr("VirtualGlass file (*.glass)")); //avoid open non .glass files
	openFileDialog.setFileMode(QFileDialog::ExistingFiles);

	QPushButton *mergeButton =  new QPushButton(&openFileDialog);
	mergeButton->setText("Merge"); //set button text
	QGridLayout *layout = (QGridLayout*)openFileDialog.layout();
	layout->addWidget(mergeButton, 4, 2); //set position
	signalMapper = new QSignalMapper();
	connect(mergeButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
	signalMapper->setMapping(mergeButton, &openFileDialog);
	connect(signalMapper, SIGNAL(mapped(QWidget*)),SLOT(castMergeButton(QWidget*)));
	QStringList list;
	//detects a click on the cancel button
	if (openFileDialog.exec()){
		list = openFileDialog.selectedFiles(); //get the selected files after click open
		if(getMerge()==false){
			open(list, false);
		}
		setViewMode(EMPTY_VIEW_MODE);
		setMerge(false);
	}
}

QString MainWindow::writeJson(Json::Value root){
	Json::StyledWriter writer;
	std::string outputConfig = writer.write( root );
	QString output = QString::fromStdString(outputConfig);
	return output;
}

void MainWindow::setupMenus()
{
	//open
	openAct = new QAction(tr("&Open"), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("Open an existing file"));

	//new
	newAct = new QAction(tr("&New"), this);
	newAct->setShortcuts(QKeySequence::New);
	newAct->setStatusTip(tr("Open a new VirtualGlass session"));

	//import svg cane
	importAct = new QAction(tr("&Import SVG Cane"), this);
	importAct->setStatusTip(tr("Import cane cross section from file."));

	//save
	saveAllAct = new QAction(tr("&Save"), this);
	saveAllAct->setShortcuts(QKeySequence::Save);
	saveAllAct->setStatusTip(tr("Save all glass to disk"));

	//saveAllAs
	saveAllAsAct = new QAction(tr("Save &As"), this);
	saveAllAsAct->setShortcuts(QKeySequence::SaveAs);
	saveAllAsAct->setStatusTip(tr("Save all glass to disk"));

	//saveSelectedAs
	saveSelectedAsAct = new QAction(tr("Save Selected As"), this);
	saveSelectedAsAct->setStatusTip(tr("Save selected glass to disk"));

	//exit
	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcuts(QKeySequence::Quit);
	exitAct->setStatusTip(tr("Exit"));

	//File menu
	fileMenu = menuBar()->addMenu(tr("&File")); //create File menu
	fileMenu->addAction(openAct); //add openButton
	fileMenu->addAction(newAct); //add newButton
	fileMenu->addSeparator();
	fileMenu->addAction(importAct); //add importButton
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
