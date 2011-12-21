

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
        move(0, 0);
        showMaximized();

        seedEverything();
        initializeRandomPiece();
        editorStack->setCurrentIndex(EMPTY_MODE); // end in pull plan mode
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
	pieceEditorWidget->setPickupTemplate(new PickupTemplate(VERTICALS_TEMPLATE));
	pieceEditorWidget->setPickupTemplateParameter(0, 14); // set number of subpulls
	pieceEditorWidget->setPickupSubplans(pullPlanEditorWidget->getPlan());
	emit someDataChanged();
}

void MainWindow :: unhighlightAllLibraryWidgets()
{
	QLayoutItem* w;
	for (int j = 1; j <= colorBarCount; ++j)
	{
		w = tableGridLayout->itemAtPosition(j , 0);
		if(dynamic_cast<QWidgetItem *>(w))
			unhighlightLibraryWidget(dynamic_cast<ColorBarLibraryWidget*>(w->widget()));
	}
	for (int j = 1; j <= pullPlanCount; ++j)
	{
		w = tableGridLayout->itemAtPosition(j , 1);
		if(dynamic_cast<QWidgetItem *>(w))
			unhighlightLibraryWidget(dynamic_cast<PullPlanLibraryWidget*>(w->widget()));
	}
	for (int j = 1; j <= pieceCount; ++j)
	{
		w = tableGridLayout->itemAtPosition(j , 2);
		if(dynamic_cast<QWidgetItem *>(w))
			unhighlightLibraryWidget(dynamic_cast<PieceLibraryWidget*>(w->widget()));
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
	drag->setHotSpot(QPoint(50, 50));

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
	QWidget* tableWidget = new QWidget(centralWidget);
	centralLayout->addWidget(tableWidget);
	QVBoxLayout* tableLayout = new QVBoxLayout(tableWidget);
	tableWidget->setLayout(tableLayout);

	QScrollArea* tableGridLibraryScrollArea = new QScrollArea(tableWidget);
	tableGridLibraryScrollArea->setBackgroundRole(QPalette::Dark);
	tableGridLibraryScrollArea->setWidgetResizable(true);
	tableGridLibraryScrollArea->setFixedWidth(370);
	tableGridLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	tableGridLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	tableLayout->addWidget(tableGridLibraryScrollArea, 1);

	QWidget* tableGridLibraryWidget = new QWidget(tableWidget);
	tableGridLibraryScrollArea->setWidget(tableGridLibraryWidget);
	tableGridLayout = new QGridLayout(tableGridLibraryWidget);
	tableGridLibraryWidget->setLayout(tableGridLayout);
	colorBarCount = pullPlanCount = pieceCount = 0;

	newColorBarButton = new QPushButton("New Color");
	newPullPlanButton = new QPushButton("New Cane");
	newPieceButton = new QPushButton("New Piece");
	tableGridLayout->setAlignment(Qt::AlignTop);
	tableGridLayout->addWidget(newColorBarButton, 0, 0);
	tableGridLayout->addWidget(newPullPlanButton, 0, 1);
	tableGridLayout->addWidget(newPieceButton, 0, 2);

	// make three qlabels for a legend
	QGridLayout* legendLayout = new QGridLayout(tableWidget);
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
	tableLayout->addLayout(legendLayout, 0);

	QLabel* descriptionLabel = new QLabel("Library - click to edit or drag to add.",
		tableWidget);
	descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	tableLayout->addWidget(descriptionLabel, 0);
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
        ++pieceCount;
        tableGridLayout->addWidget(pieceEditorPieceLibraryWidget, pieceCount, 2);
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
	++colorBarCount;
	tableGridLayout->addWidget(colorEditorBarLibraryWidget, colorBarCount, 0);
}

void MainWindow :: setupPullPlanEditor()
{
	// Setup data objects - the current plan and library widget for this plan
	pullPlanEditorWidget = new PullPlanEditorWidget(editorStack);
	pullPlanEditorPlanLibraryWidget = new PullPlanLibraryWidget(pullPlanEditorWidget->getPlan());
	++pullPlanCount;
	tableGridLayout->addWidget(pullPlanEditorPlanLibraryWidget, pullPlanCount, 1);
}

void MainWindow :: newPiece()
{
	Piece* oldEditorPiece = pieceEditorWidget->getPiece();
	
	// Create the new piece
	Piece* newEditorPiece = oldEditorPiece->copy();

	// Create the new library entry
	unhighlightAllLibraryWidgets();
	pieceEditorPieceLibraryWidget = new PieceLibraryWidget(newEditorPiece);
	++pieceCount;
	tableGridLayout->addWidget(pieceEditorPieceLibraryWidget, pieceCount, 2);

	pieceEditorWidget->setPiece(newEditorPiece);

	// Load up the right editor
	editorStack->setCurrentIndex(PIECE_MODE);

	emit someDataChanged();
}

void MainWindow :: newColorBar()
{
	PullPlan* oldEditorBar = colorEditorWidget->getColorBar();

	PullPlan* newEditorBar = oldEditorBar->copy();

	Color* newColor = new Color;
	*(newColor) = *(oldEditorBar->color);
	newEditorBar->color = newColor;

	// Create the new library entry
	unhighlightAllLibraryWidgets();
	colorEditorBarLibraryWidget = new ColorBarLibraryWidget(newEditorBar);
	++colorBarCount;
	tableGridLayout->addWidget(colorEditorBarLibraryWidget, colorBarCount, 0);

	// Give the new plan to the editor
	colorEditorWidget->setColorBar(newEditorBar);

	// Load up the right editor
	editorStack->setCurrentIndex(COLORBAR_MODE);

	// Trigger GUI updates
	emit someDataChanged();
}

void MainWindow :: newPullPlan()
{
	PullPlan* oldEditorPlan = pullPlanEditorWidget->getPlan();

	// Create the new plan
	PullPlan* newEditorPlan = oldEditorPlan->copy();

	// Create the new library entry
	unhighlightAllLibraryWidgets();
	pullPlanEditorPlanLibraryWidget = new PullPlanLibraryWidget(newEditorPlan);
	++pullPlanCount;
	tableGridLayout->addWidget(pullPlanEditorPlanLibraryWidget, pullPlanCount, 1);

	// Give the new plan to the editor
	pullPlanEditorWidget->setPlan(newEditorPlan);

	// Load up the right editor
	editorStack->setCurrentIndex(PULLPLAN_MODE);

	// Trigger GUI updates
	emit someDataChanged();
}

void MainWindow :: newPullPlan(PullPlan* newPlan)
{
        unhighlightAllLibraryWidgets();
        pullPlanEditorPlanLibraryWidget = new PullPlanLibraryWidget(newPlan);
        ++pullPlanCount;
        tableGridLayout->addWidget(pullPlanEditorPlanLibraryWidget, pullPlanCount, 1);

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
			for (int i = 1; i <= pullPlanCount; ++i)
			{
				pplw = dynamic_cast<PullPlanLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(tableGridLayout->itemAtPosition(i , 1))->widget());
				if (pplw->getPullPlan()->hasDependencyOn(colorEditorBarLibraryWidget->getPullPlan()->color))
					highlightLibraryWidget(pplw, USES_DEPENDANCY);
			}

			PieceLibraryWidget* plw;
			for (int i = 1; i <= pieceCount; ++i)
			{
				plw = dynamic_cast<PieceLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(tableGridLayout->itemAtPosition(i , 2))->widget());
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
			for (int i = 1; i <= colorBarCount; ++i)
			{
				cblw = dynamic_cast<ColorBarLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(tableGridLayout->itemAtPosition(i , 0))->widget());
				if (pullPlanEditorWidget->getPlan()->hasDependencyOn(cblw->getPullPlan()->color))
					highlightLibraryWidget(cblw, IS_USED_BY_DEPENDANCY);
			}

			PullPlanLibraryWidget* pplw;
			for (int i = 1; i <= pullPlanCount; ++i)
			{
				pplw = dynamic_cast<PullPlanLibraryWidget*>(
						dynamic_cast<QWidgetItem *>(tableGridLayout->itemAtPosition(i , 1))->widget());
				if (pullPlanEditorWidget->getPlan()->hasDependencyOn(pplw->getPullPlan()))
						highlightLibraryWidget(pplw, IS_USED_BY_DEPENDANCY);
				else if (pplw->getPullPlan()->hasDependencyOn(pullPlanEditorWidget->getPlan()))
						highlightLibraryWidget(pplw, USES_DEPENDANCY);
			}

			PieceLibraryWidget* plw;
			for (int i = 1; i <= pieceCount; ++i)
			{
				plw = dynamic_cast<PieceLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(tableGridLayout->itemAtPosition(i , 2))->widget());
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
			for (int i = 1; i <= colorBarCount; ++i)
			{
				cblw = dynamic_cast<ColorBarLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(tableGridLayout->itemAtPosition(i , 0))->widget());
				if (pieceEditorWidget->getPiece()->hasDependencyOn(cblw->getPullPlan()->color))
					highlightLibraryWidget(cblw, IS_USED_BY_DEPENDANCY);
			}

			PullPlanLibraryWidget* pplw;
			for (int i = 1; i <= pullPlanCount; ++i)
			{
				pplw = dynamic_cast<PullPlanLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(tableGridLayout->itemAtPosition(i , 1))->widget());
				if (pieceEditorWidget->getPiece()->hasDependencyOn(pplw->getPullPlan()))
					highlightLibraryWidget(pplw, IS_USED_BY_DEPENDANCY);
			}

			highlightLibraryWidget(pieceEditorPieceLibraryWidget, IS_DEPENDANCY);
			break;
		}
	}
}


