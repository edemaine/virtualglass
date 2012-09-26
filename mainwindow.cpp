

#include "mainwindow.h"

MainWindow :: MainWindow()
{
	centralWidget = new QWidget(this);
	this->setCentralWidget(centralWidget);

	centralLayout = new QHBoxLayout(centralWidget);
	setupLibrary();
	setupEditors();
	setupConnections();

	setWindowTitle(windowTitle());
	show();

	seedEverything();
	setViewMode(EMPTY_VIEW_MODE);
	emit someDataChanged();
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
	colorEditorWidget->seedColors();
	emit someDataChanged();

	// Load pull template types
	setViewMode(PULLPLAN_VIEW_MODE);
	emit someDataChanged();
	pullPlanEditorWidget->seedTemplates();

	// Load pickup and piece template types
	setViewMode(PIECE_VIEW_MODE);
	emit someDataChanged();
	pieceEditorWidget->seedTemplates();
}

void MainWindow :: unhighlightAllLibraryWidgets()
{
	QLayoutItem* w;
	for (int j = 0; j < colorBarLibraryLayout->count(); ++j)
	{
		w = colorBarLibraryLayout->itemAt(j);
		unhighlightLibraryWidget(dynamic_cast<AsyncColorBarLibraryWidget*>(w->widget()));
	}
	for (int j = 0; j < pullPlanLibraryLayout->count(); ++j)
	{
		w = pullPlanLibraryLayout->itemAt(j);
		unhighlightLibraryWidget(dynamic_cast<AsyncPullPlanLibraryWidget*>(w->widget()));
	}
	for (int j = 0; j < pieceLibraryLayout->count(); ++j)
	{
		w = pieceLibraryLayout->itemAt(j);
		unhighlightLibraryWidget(dynamic_cast<AsyncPieceLibraryWidget*>(w->widget()));
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
		unhighlightAllLibraryWidgets();
		colorEditorWidget->setGlassColor(cblw->getGlassColor());
		setViewMode(COLORBAR_VIEW_MODE);
		emit someDataChanged();
	}
	else if (plplw != NULL)
	{
		unhighlightAllLibraryWidgets();
		pullPlanEditorWidget->setPlan(plplw->getPullPlan());
		setViewMode(PULLPLAN_VIEW_MODE);
		emit someDataChanged();
	}
	else if (plw != NULL)
	{
		unhighlightAllLibraryWidgets();
		pieceEditorWidget->setPiece(plw->getPiece());
		setViewMode(PIECE_VIEW_MODE);
		emit someDataChanged();
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
	l2->setStyleSheet("border: 3px solid " + QColor(0, 0, 255, 255).name() + ";");
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
	pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(pieceEditorWidget->getPiece()));
}

void MainWindow :: setupEmptyPaneEditor()
{
	emptyEditorPage = new QWidget(editorStack);
	QHBoxLayout* editorLayout = new QHBoxLayout(emptyEditorPage);
	emptyEditorPage->setLayout(editorLayout);
	whatToDoLabel = new QLabel("Loading...", emptyEditorPage);
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
	unhighlightAllLibraryWidgets();
	pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(newEditorPiece));
	pieceEditorWidget->setPiece(newEditorPiece);

	// Load up the right editor
	setViewMode(PIECE_VIEW_MODE);

	emit someDataChanged();
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

void MainWindow :: newColorBar()
{
	GlassColor* newGlassColor = new GlassColor();

	// Create the new library entry
	unhighlightAllLibraryWidgets();
	colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(newGlassColor, this));
	colorEditorWidget->setGlassColor(newGlassColor);

	// Load up the right editor
	setViewMode(COLORBAR_VIEW_MODE);

	// Trigger GUI updates
	emit someDataChanged();
}

void MainWindow :: copyColorBar()
{
	if (editorStack->currentIndex() != COLORBAR_VIEW_MODE)
		return;

	GlassColor* newEditorGlassColor = colorEditorWidget->getGlassColor()->copy();

	// Create the new library entry
	unhighlightAllLibraryWidgets();
	colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(newEditorGlassColor, this));
	colorEditorWidget->setGlassColor(newEditorGlassColor);

	// Trigger GUI updates
	emit someDataChanged();
}

void MainWindow :: newPullPlan()
{
	PullPlan *newEditorPlan = new PullPlan(PullTemplate::BASE_CIRCLE);
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
	unhighlightAllLibraryWidgets();
	pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(newPlan));

	// Give the new plan to the editor
	pullPlanEditorWidget->setPlan(newPlan);

	// Load up the right editor
	setViewMode(PULLPLAN_VIEW_MODE);

	// Trigger GUI updates
	emit someDataChanged();
}


void MainWindow :: unhighlightLibraryWidget(PieceTemplateLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
}

void MainWindow :: unhighlightLibraryWidget(PickupTemplateLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
}

void MainWindow :: unhighlightLibraryWidget(AsyncColorBarLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
}

void MainWindow :: unhighlightLibraryWidget(AsyncPullPlanLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
}

void MainWindow :: unhighlightLibraryWidget(AsyncPieceLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
}

void MainWindow :: highlightLibraryWidget(PieceTemplateLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
	((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(IS_DEPENDANCY);
	w->graphicsEffect()->setEnabled(true);
}

void MainWindow :: highlightLibraryWidget(PickupTemplateLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
	((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(IS_DEPENDANCY);
	w->graphicsEffect()->setEnabled(true);
}

void MainWindow :: highlightLibraryWidget(AsyncColorBarLibraryWidget* w, enum Dependancy d)
{
	w->graphicsEffect()->setEnabled(false);
	((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(d);
	w->graphicsEffect()->setEnabled(true);
}

void MainWindow :: highlightLibraryWidget(AsyncPullPlanLibraryWidget* w, enum Dependancy d)
{
	w->graphicsEffect()->setEnabled(false);
	((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(d);
	w->graphicsEffect()->setEnabled(true);
}

void MainWindow :: highlightLibraryWidget(AsyncPieceLibraryWidget* w, enum Dependancy d)
{
	w->graphicsEffect()->setEnabled(false);
	((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(d);
	w->graphicsEffect()->setEnabled(true);
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
                                        highlightLibraryWidget(cblw, IS_DEPENDANCY);
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
					highlightLibraryWidget(pplw, USES_DEPENDANCY);
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
					highlightLibraryWidget(plw, USES_DEPENDANCY);
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
					highlightLibraryWidget(cblw, USEDBY_DEPENDANCY);
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
					highlightLibraryWidget(pplw, IS_DEPENDANCY);
				}
				else if (pullPlanEditorWidget->getPlan()->hasDependencyOn(pplw->getPullPlan()))
				{
					highlightLibraryWidget(pplw, USEDBY_DEPENDANCY);
				}
				else if (pplw->getPullPlan()->hasDependencyOn(pullPlanEditorWidget->getPlan()))	
				{
					pplw->updatePixmaps();
					highlightLibraryWidget(pplw, USES_DEPENDANCY);
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
					highlightLibraryWidget(plw, USES_DEPENDANCY);
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
					highlightLibraryWidget(cblw, USEDBY_DEPENDANCY);
			}

			AsyncPullPlanLibraryWidget* pplw;
			for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
				if (pieceEditorWidget->getPiece()->hasDependencyOn(pplw->getPullPlan()))
					highlightLibraryWidget(pplw, USEDBY_DEPENDANCY);
			}

                        AsyncPieceLibraryWidget* plw;
                        for (int i = 0; i < pieceLibraryLayout->count(); ++i)
                        {
                                plw = dynamic_cast<AsyncPieceLibraryWidget*>(
                                        dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
                                if (plw->getPiece() == pieceEditorWidget->getPiece())
                                {
                                        plw->updatePixmap();
                                        highlightLibraryWidget(plw, IS_DEPENDANCY);
                                }
                        }

			break;
		}
	}
}


