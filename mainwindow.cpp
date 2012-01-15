

#include "mainwindow.h"

MainWindow :: MainWindow()
{
	centralWidget = new QWidget(this);
	this->setCentralWidget(centralWidget);

	centralLayout = new QHBoxLayout(centralWidget);
	setupLibrary();
	setupStatusBar();
	setupEditors();
	setupConnections();

	setWindowTitle(tr("VirtualGlass"));
	//HACK for video recording:
	//setFixedSize(1600,900);
        //move(-8, 0);
        show();

        seedEverything();
        //initializeRandomPiece();
        //editorStack->setCurrentIndex(EMPTY_MODE); // end in pull plan mode
        emit someDataChanged();
        whatToDoLabel->setText("Click a library item at left to edit/view.");
}

void MainWindow :: seedEverything()
{
	// Load color stuff
	editorStack->setCurrentIndex(COLORBAR_MODE);
	colorEditorWidget->seedColors();
	emit someDataChanged();

	// Load pull template types
	editorStack->setCurrentIndex(PULLPLAN_MODE);
	emit someDataChanged();
	pullPlanEditorWidget->seedTemplates();

        // Load pickup and piece template types
	editorStack->setCurrentIndex(PIECE_MODE);
	emit someDataChanged();
	pieceEditorWidget->seedTemplates();
}

// Too weird to live; too strange to die
void MainWindow :: initializeRandomPiece()
{
	// Setup colors
	editorStack->setCurrentIndex(COLORBAR_MODE); 
	PullPlan* clearColorBar = colorEditorWidget->getColorBar();
	newColorBar();
	colorEditorWidget->setColor(1.0, 1.0, 1.0, 1.0);
	PullPlan* opaqueColorBar = colorEditorWidget->getColorBar();
	newColorBar();
	PullPlan* transparentColorBar = colorEditorWidget->getColorBar();
	colorEditorWidget->setColor((qrand() % 255) / 255.0,
		(qrand() % 255) / 255.0, 
		(qrand() % 255) / 255.0,
		(qrand() % 128 + 70) / 255.0);

	// Setup cane
	editorStack->setCurrentIndex(PULLPLAN_MODE); 
	pullPlanEditorWidget->setPlanTemplate(new PullTemplate(CASED_CIRCLE_PULL_TEMPLATE));
	pullPlanEditorWidget->setPlanTemplateCasingThickness(0.5);
	pullPlanEditorWidget->setPlanSubplans(opaqueColorBar);
	pullPlanEditorWidget->setPlanColor(clearColorBar->color);
	PullPlan* subplan = pullPlanEditorWidget->getPlan();
	newPullPlan();
	pullPlanEditorWidget->setPlanTemplate(new PullTemplate(HORIZONTAL_LINE_CIRCLE_PULL_TEMPLATE));
	pullPlanEditorWidget->setPlanTemplateCasingThickness((qrand() % 30 + 40) / 100.0);
	pullPlanEditorWidget->setPlanSubplans(subplan);
	pullPlanEditorWidget->setPlanColor(transparentColorBar->color);
	pullPlanEditorWidget->setPlanTwist(20);
	emit someDataChanged();

	// Setup piece
	editorStack->setCurrentIndex(PIECE_MODE); 
	pieceEditorWidget->setPieceTemplate(
		new PieceTemplate((qrand() % (LAST_PIECE_TEMPLATE - FIRST_PIECE_TEMPLATE + 1)) + FIRST_PIECE_TEMPLATE));
	pieceEditorWidget->setPickupTemplate(new PickupTemplate(VERTICALS_PICKUP_TEMPLATE));
	pieceEditorWidget->setPickupTemplateParameter(0, 14); // set number of subpulls
	pieceEditorWidget->setPickupSubplans(pullPlanEditorWidget->getPlan());
	emit someDataChanged();
}

void MainWindow :: unhighlightAllLibraryWidgets()
{
	QLayoutItem* w;
	for (int j = 0; j < colorBarLibraryLayout->count(); ++j)
	{
		w = colorBarLibraryLayout->itemAt(j);
		unhighlightLibraryWidget(dynamic_cast<ColorBarLibraryWidget*>(w->widget()));
	}
	for (int j = 0; j < pullPlanLibraryLayout->count(); ++j)
	{
		w = pullPlanLibraryLayout->itemAt(j);
		unhighlightLibraryWidget(dynamic_cast<PullPlanLibraryWidget*>(w->widget()));
	}
	for (int j = 0; j < pieceLibraryLayout->count(); ++j)
	{
		w = pieceLibraryLayout->itemAt(j);
		unhighlightLibraryWidget(dynamic_cast<PieceLibraryWidget*>(w->widget()));
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
		case COLORBAR_MODE:
		{
			if (colorBarLibraryLayout->count() == 1)
				return;

			int i;
			for (i = 0; i < colorBarLibraryLayout->count(); ++i)
			{
				w = colorBarLibraryLayout->itemAt(i);
				PullPlan* p = dynamic_cast<ColorBarLibraryWidget*>(w->widget())->getPullPlan();
				if (p == colorEditorWidget->getColorBar())
				{
					// this may be a memory leak, the library widget is never explicitly deleted
					w = colorBarLibraryLayout->takeAt(i); 
					delete w->widget();
					delete w;
					break;	
				}
			}

			colorEditorBarLibraryWidget = dynamic_cast<ColorBarLibraryWidget*>(
								colorBarLibraryLayout->itemAt(
									MIN(colorBarLibraryLayout->count()-1, i))->widget());
			colorEditorWidget->setColorBar(colorEditorBarLibraryWidget->getPullPlan());
			emit someDataChanged();
			break;
		}
		case PULLPLAN_MODE:
		{
			if (pullPlanLibraryLayout->count() == 1)
				return;

                        int i;
                        for (i = 0; i < pullPlanLibraryLayout->count(); ++i)
                        {
                                w = pullPlanLibraryLayout->itemAt(i);
                                PullPlan* p = dynamic_cast<PullPlanLibraryWidget*>(w->widget())->getPullPlan();
                                if (p == pullPlanEditorWidget->getPlan())
                                {
                                        // this may be a memory leak, the library widget is never explicitly deleted
                                        w = pullPlanLibraryLayout->takeAt(i);
                                        delete w->widget();
                                        delete w;
                                        break;
                                }
                        }

                        pullPlanEditorPlanLibraryWidget = dynamic_cast<PullPlanLibraryWidget*>(
                                                          	pullPlanLibraryLayout->itemAt(
                                                                        MIN(pullPlanLibraryLayout->count()-1, i))->widget());
                        pullPlanEditorWidget->setPlan(pullPlanEditorPlanLibraryWidget->getPullPlan());
                        emit someDataChanged();
                        break;
		}
		case PIECE_MODE:
		{
			if (pieceLibraryLayout->count() == 1)
				return;

                        int i;
                        for (i = 0; i < pieceLibraryLayout->count(); ++i)
                        {
                                w = pieceLibraryLayout->itemAt(i);
                                Piece* p = dynamic_cast<PieceLibraryWidget*>(w->widget())->getPiece();
                                if (p == pieceEditorWidget->getPiece())
                                {
                                        // this may be a memory leak, the library widget is never explicitly deleted
                                        w = pieceLibraryLayout->takeAt(i);
                                        delete w->widget();
                                        delete w;
                                        break;
                                }
                        }

                        pieceEditorPieceLibraryWidget = dynamic_cast<PieceLibraryWidget*>(
                                                                pieceLibraryLayout->itemAt(
                                                                        MIN(pieceLibraryLayout->count()-1, i))->widget());
                        pieceEditorWidget->setPiece(pieceEditorPieceLibraryWidget->getPiece());
                        emit someDataChanged();
                        break;
		}
	}
	
}

void MainWindow :: mouseReleaseEvent(QMouseEvent* event)
{
	// If this is a drag and not the end of a click, don't process (dropEvent will do it instead)
	if ((event->pos() - dragStartPosition).manhattanLength() > QApplication::startDragDistance())
	{
		return;
	}

	ColorBarLibraryWidget* cblw = dynamic_cast<ColorBarLibraryWidget*>(childAt(event->pos()));
	PullPlanLibraryWidget* plplw = dynamic_cast<PullPlanLibraryWidget*>(childAt(event->pos()));
	PieceLibraryWidget* plw = dynamic_cast<PieceLibraryWidget*>(childAt(event->pos()));

	if (cblw != NULL)
	{
		unhighlightAllLibraryWidgets();
		colorEditorBarLibraryWidget = cblw;
		colorEditorWidget->setColorBar(cblw->getPullPlan());
		editorStack->setCurrentIndex(COLORBAR_MODE);
		emit someDataChanged();
	}
	else if (plplw != NULL)
	{
		unhighlightAllLibraryWidgets();
		pullPlanEditorPlanLibraryWidget = plplw;
		pullPlanEditorWidget->setPlan(plplw->getPullPlan());
		editorStack->setCurrentIndex(PULLPLAN_MODE);
		emit someDataChanged();
	}
	else if (plw != NULL)
	{
		unhighlightAllLibraryWidgets();
		pieceEditorPieceLibraryWidget = plw;
		pieceEditorWidget->setPiece(plw->getPiece());
		editorStack->setCurrentIndex(PIECE_MODE);
		emit someDataChanged();
	}
}


void MainWindow :: mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
		this->dragStartPosition = event->pos();
}

void MainWindow :: mouseMoveEvent(QMouseEvent* event)
{
	void* plan = NULL;
	QPixmap pixmap;

	if (!(event->buttons() & Qt::LeftButton))
		return;
	if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
		return;

	statusBar->showMessage("Hold Shift to fill all subcanes at once, Alt to fill every other subcane.", 10000);

	ColorBarLibraryWidget* cblw = dynamic_cast<ColorBarLibraryWidget*>(childAt(event->pos()));
	PullPlanLibraryWidget* plplw = dynamic_cast<PullPlanLibraryWidget*>(childAt(event->pos()));
	int type;
	if (cblw != NULL)
	{
		plan = cblw->getPullPlan();
		pixmap = *cblw->getEditorPixmap();
		type = COLOR_BAR_MIME;
	}
	else if (plplw != NULL)
	{
		plan = plplw->getPullPlan();
		pixmap = *plplw->getEditorPixmap();
		type = PULL_PLAN_MIME;
	}
	else
		return;

	char buf[500];
	sprintf(buf, "%p %d", plan, type);
	QByteArray pointerData(buf);
	QMimeData* mimeData = new QMimeData;
	mimeData->setText(pointerData);

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);
	drag->setPixmap(pixmap);

	drag->exec(Qt::CopyAction);

}

void MainWindow :: dragMoveEvent(QDragMoveEvent* event)
{
	event->acceptProposedAction();
}

void MainWindow :: setupConnections()
{
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

	connect(newColorBarButton, SIGNAL(pressed()), this, SLOT(newColorBar()));
	connect(colorEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

	connect(newPullPlanButton, SIGNAL(pressed()), this, SLOT(newPullPlan()));
	connect(pullPlanEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(pullPlanEditorWidget, SIGNAL(newPullPlan(PullPlan*)), this, SLOT(newPullPlan(PullPlan*)));

	connect(newPieceButton, SIGNAL(pressed()), this, SLOT(newPiece()));
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

	colorBarLibraryLayout = new QVBoxLayout(libraryWidget);
	colorBarLibraryLayout->setDirection(QBoxLayout::BottomToTop);
	pullPlanLibraryLayout = new QVBoxLayout(libraryWidget);
	pullPlanLibraryLayout->setDirection(QBoxLayout::BottomToTop);
	pieceLibraryLayout = new QVBoxLayout(libraryWidget);
	pieceLibraryLayout->setDirection(QBoxLayout::BottomToTop);
	superlibraryLayout->addLayout(colorBarLibraryLayout, 1, 0, Qt::AlignTop);
	superlibraryLayout->addLayout(pullPlanLibraryLayout, 1, 1, Qt::AlignTop);
	superlibraryLayout->addLayout(pieceLibraryLayout, 1, 2, Qt::AlignTop);

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

void MainWindow :: setupStatusBar()
{
	statusBar = new QStatusBar();
	this->setStatusBar(statusBar);
}

void MainWindow :: setupEditors()
{
	editorStack = new QStackedWidget(centralWidget);
	centralLayout->addWidget(editorStack, 2);

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
        pieceEditorPieceLibraryWidget = new PieceLibraryWidget(pieceEditorWidget->getPiece());
        pieceLibraryLayout->addWidget(pieceEditorPieceLibraryWidget);
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
	colorEditorWidget = new ColorEditorWidget(editorStack);
	colorEditorBarLibraryWidget = new ColorBarLibraryWidget(colorEditorWidget->getColorBar());
	colorBarLibraryLayout->addWidget(colorEditorBarLibraryWidget);
}

void MainWindow :: setupPullPlanEditor()
{
	// Setup data objects - the current plan and library widget for this plan
	pullPlanEditorWidget = new PullPlanEditorWidget(editorStack);
	pullPlanEditorPlanLibraryWidget = new PullPlanLibraryWidget(pullPlanEditorWidget->getPlan());
	pullPlanLibraryLayout->addWidget(pullPlanEditorPlanLibraryWidget);
}

void MainWindow :: newPiece()
{
	// Create the new piece
	Piece* newEditorPiece = new Piece(TUMBLER_PIECE_TEMPLATE);

	// Create the new library entry
	unhighlightAllLibraryWidgets();
	pieceEditorPieceLibraryWidget = new PieceLibraryWidget(newEditorPiece);
	pieceLibraryLayout->addWidget(pieceEditorPieceLibraryWidget);

	pieceEditorWidget->setPiece(newEditorPiece);

	// Load up the right editor
	editorStack->setCurrentIndex(PIECE_MODE);

	emit someDataChanged();
}

void MainWindow :: newColorBar()
{
	PullPlan* oldEditorBar = colorEditorWidget->getColorBar();

	PullPlan* newEditorBar = oldEditorBar->copy();

	//new color is always clear:
	Color* newColor = new Color;
	*newColor = make_vector(1.0f, 1.0f, 1.0f, 0.0f);
	//*(newColor) = *(oldEditorBar->color);
	newEditorBar->color = newColor;

	// Create the new library entry
	unhighlightAllLibraryWidgets();
	colorEditorBarLibraryWidget = new ColorBarLibraryWidget(newEditorBar);
	colorBarLibraryLayout->addWidget(colorEditorBarLibraryWidget);

	// Give the new plan to the editor
	colorEditorWidget->setColorBar(newEditorBar);

	// Load up the right editor
	editorStack->setCurrentIndex(COLORBAR_MODE);

	// Trigger GUI updates
	emit someDataChanged();
}

void MainWindow :: newPullPlan()
{
	/*
	 This (old) code duplicates the current cane:
	 PullPlan* oldEditorPlan = pullPlanEditorWidget->getPlan();
	 // Create the new plan
	 PullPlan* newEditorPlan = oldEditorPlan->copy();
	*/

	Color* color = new Color;
	*color = make_vector(1.0f, 1.0f, 1.0f, 0.0f); //clear
	PullPlan *newEditorPlan = new PullPlan(CIRCLE_BASE_PULL_TEMPLATE, color);

	newEditorPlan->setTemplate(new PullTemplate(CASED_CIRCLE_PULL_TEMPLATE));

	newPullPlan(newEditorPlan);
}

void MainWindow :: newPullPlan(PullPlan* newPlan)
{
        unhighlightAllLibraryWidgets();
        pullPlanEditorPlanLibraryWidget = new PullPlanLibraryWidget(newPlan);
        pullPlanLibraryLayout->addWidget(pullPlanEditorPlanLibraryWidget);

        // Give the new plan to the editor
        pullPlanEditorWidget->setPlan(newPlan);

        // Load up the right editor
        editorStack->setCurrentIndex(PULLPLAN_MODE);

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

void MainWindow :: unhighlightLibraryWidget(ColorBarLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
}

void MainWindow :: unhighlightLibraryWidget(PullPlanLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
}

void MainWindow :: unhighlightLibraryWidget(PieceLibraryWidget* w)
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

void MainWindow :: highlightLibraryWidget(ColorBarLibraryWidget* w, int dependancy)
{
	w->graphicsEffect()->setEnabled(false);
	((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(dependancy);
	w->graphicsEffect()->setEnabled(true);
}

void MainWindow :: highlightLibraryWidget(PullPlanLibraryWidget* w, int dependancy)
{
	w->graphicsEffect()->setEnabled(false);
	((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(dependancy);
	w->graphicsEffect()->setEnabled(true);
}

void MainWindow :: highlightLibraryWidget(PieceLibraryWidget* w, int dependancy)
{
	w->graphicsEffect()->setEnabled(false);
	((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(dependancy);
	w->graphicsEffect()->setEnabled(true);
}

void MainWindow :: updateEverything()
{
	switch (editorStack->currentIndex())
	{
		case COLORBAR_MODE:
			colorEditorWidget->updateEverything();
			break;
		case PULLPLAN_MODE:
			pullPlanEditorWidget->updateEverything();
			break;
		case PIECE_MODE:
			pieceEditorWidget->updateEverything();
			break;
	}

	updateLibrary();
}

void MainWindow :: updateLibrary()
{
	unhighlightAllLibraryWidgets();

	switch (editorStack->currentIndex())
	{
		case COLORBAR_MODE:
		{
			colorEditorWidget->updateLibraryWidgetPixmaps(colorEditorBarLibraryWidget);

			PullPlanLibraryWidget* pplw;
			for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				pplw = dynamic_cast<PullPlanLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
				if (pplw->getPullPlan()->hasDependencyOn(colorEditorBarLibraryWidget->getPullPlan()->color))
					highlightLibraryWidget(pplw, USES_DEPENDANCY);
			}

			PieceLibraryWidget* plw;
			for (int i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				plw = dynamic_cast<PieceLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
				if (plw->getPiece()->hasDependencyOn(colorEditorBarLibraryWidget->getPullPlan()->color))
					highlightLibraryWidget(plw, USES_DEPENDANCY);
			}

			highlightLibraryWidget(colorEditorBarLibraryWidget, IS_DEPENDANCY);
			break;
		}
		case PULLPLAN_MODE:
		{
			pullPlanEditorWidget->updateLibraryWidgetPixmaps(pullPlanEditorPlanLibraryWidget);

			ColorBarLibraryWidget* cblw;
			for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
			{
				cblw = dynamic_cast<ColorBarLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
				if (pullPlanEditorWidget->getPlan()->hasDependencyOn(cblw->getPullPlan()->color))
					highlightLibraryWidget(cblw, IS_USED_BY_DEPENDANCY);
			}

			PullPlanLibraryWidget* pplw;
			for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				pplw = dynamic_cast<PullPlanLibraryWidget*>(
						dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
				if (pullPlanEditorWidget->getPlan()->hasDependencyOn(pplw->getPullPlan()))
						highlightLibraryWidget(pplw, IS_USED_BY_DEPENDANCY);
				else if (pplw->getPullPlan()->hasDependencyOn(pullPlanEditorWidget->getPlan()))
						highlightLibraryWidget(pplw, USES_DEPENDANCY);
			}

			PieceLibraryWidget* plw;
			for (int i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				plw = dynamic_cast<PieceLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
				if (plw->getPiece()->hasDependencyOn(pullPlanEditorWidget->getPlan()))
					highlightLibraryWidget(plw, USES_DEPENDANCY);
			}

			highlightLibraryWidget(pullPlanEditorPlanLibraryWidget, IS_DEPENDANCY);
			break;
		}
		case PIECE_MODE:
		{
			pieceEditorWidget->updateLibraryWidgetPixmaps(pieceEditorPieceLibraryWidget);

			ColorBarLibraryWidget* cblw;
			for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
			{
				cblw = dynamic_cast<ColorBarLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
				if (pieceEditorWidget->getPiece()->hasDependencyOn(cblw->getPullPlan()->color))
					highlightLibraryWidget(cblw, IS_USED_BY_DEPENDANCY);
			}

			PullPlanLibraryWidget* pplw;
			for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				pplw = dynamic_cast<PullPlanLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
				if (pieceEditorWidget->getPiece()->hasDependencyOn(pplw->getPullPlan()))
					highlightLibraryWidget(pplw, IS_USED_BY_DEPENDANCY);
			}

			highlightLibraryWidget(pieceEditorPieceLibraryWidget, IS_DEPENDANCY);
			break;
		}
	}
}


