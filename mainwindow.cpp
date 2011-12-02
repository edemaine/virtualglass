

#include "mainwindow.h"
#include "qgraphicshighlighteffect.h"

MainWindow :: MainWindow(Model* model)
{
	setupDone = false;
	centralWidget = new QWidget(this);
	this->setCentralWidget(centralWidget);
	this->model = model;

	centralLayout = new QHBoxLayout(centralWidget);
	setupTable();
	setupEditors();
	setupConnections();

	setWindowTitle(tr("Virtual Glass"));
	move(0, 0);
}

void MainWindow :: seedEverything()
{
	// Load color stuff
	editorStack->setCurrentIndex(COLORBAR_MODE);
	colorEditorViewWidget->seedBrandColors();
	emit someDataChanged();

	// Load pull template types
	editorStack->setCurrentIndex(PULLPLAN_MODE);
	emit someDataChanged();
	pullPlanEditorWidget->seedTemplates();

	// Load pickup template types
	editorStack->setCurrentIndex(PIECE_MODE);
	emit someDataChanged();

	for (int i = FIRST_PICKUP_TEMPLATE; i <= LAST_PICKUP_TEMPLATE; ++i)
	{
		pieceEditorPiece->pickup->setTemplate(new PickupTemplate(i));
		emit someDataChanged();
		PickupTemplateLibraryWidget *ptlw = new PickupTemplateLibraryWidget(
			pickupPlanEditorViewWidget->getPixmap().scaled(100, 100), i);
		pickupTemplateLibraryLayout->addWidget(ptlw);
	}

	//initializeRandomPiece();

	editorStack->setCurrentIndex(EMPTY_MODE); // end in pull plan mode
	emit someDataChanged();

	setupDone = true;
}

// Too weird to live; too strange to die
void MainWindow :: initializeRandomPiece()
{
	// Setup colors	
	editorStack->setCurrentIndex(COLORBAR_MODE); // end in pull plan mode
	Color* opaqueColor = colorEditorPlan->color;
	newColorBar();
	Color* transparentColor = colorEditorPlan->color;
	transparentColor->r = (qrand() % 255) / 255.0;
	transparentColor->g = (qrand() % 255) / 255.0;
	transparentColor->b = (qrand() % 255) / 255.0;
	transparentColor->a = (qrand() % 128 + 70) / 255.0;
	emit someDataChanged();

	// Setup cane 
	editorStack->setCurrentIndex(PULLPLAN_MODE); // end in pull plan mode
	pullPlanEditorWidget->setPlanTemplate(new PullTemplate(qrand() % (LAST_PULL_TEMPLATE - 3) + 3));
	pullPlanEditorWidget->setPlanTemplateCasingThickness((qrand() % 50 + 25) / 100.0);
	pullPlanEditorWidget->setPlanSubplans(opaqueColor);
	pullPlanEditorWidget->setPlanColor(transparentColor);
	pullPlanEditorWidget->setPlanTwist(20);
	emit someDataChanged();
		
	// Setup piece
	editorStack->setCurrentIndex(PIECE_MODE); // end in pull plan mode
	pieceEditorPiece->setTemplate(new PieceTemplate(qrand() % (LAST_PIECE_TEMPLATE - FIRST_PIECE_TEMPLATE + 1) + 1));
	for (unsigned int i = 0; i < pieceEditorPiece->getTemplate()->parameterValues.size(); ++i)
	{
		pieceEditorPiece->getTemplate()->parameterValues[i] = qrand() % 100;
	}
	pieceEditorPiece->pickup->setTemplate(new PickupTemplate(VERTICALS_TEMPLATE));
	pieceEditorPiece->pickup->getTemplate()->setParameter(0, 14 + qrand() % 20); // set number of subpulls
	pieceEditorPiece->pickup->subplans.clear();
	for (unsigned int i = 0; i < pieceEditorPiece->pickup->getTemplate()->subtemps.size(); ++i)
	{
		pieceEditorPiece->pickup->subplans.push_back(pullPlanEditorWidget->getPlan());
	}
	emit someDataChanged();
}

void MainWindow :: unhighlightAllPlanLibraryWidgets(bool setupDone)
{
	highlightPlanLibraryWidgets(colorEditorPlanLibraryWidget,false,setupDone);
	highlightPlanLibraryWidgets(pullPlanEditorPlanLibraryWidget,false,setupDone);
	highlightPlanLibraryWidgets(pieceEditorPieceLibraryWidget,false,setupDone);
}


void MainWindow :: mouseReleaseEvent(QMouseEvent* event)
{
	if (!((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance()))
	{
		return;
	}

	ColorBarLibraryWidget* cblw = dynamic_cast<ColorBarLibraryWidget*>(childAt(event->pos()));
	PullPlanLibraryWidget* plplw = dynamic_cast<PullPlanLibraryWidget*>(childAt(event->pos()));
	PieceLibraryWidget* plw = dynamic_cast<PieceLibraryWidget*>(childAt(event->pos()));
	PullTemplateLibraryWidget* ptlw = dynamic_cast<PullTemplateLibraryWidget*>(childAt(event->pos()));
	PickupTemplateLibraryWidget* pktlw = dynamic_cast<PickupTemplateLibraryWidget*>(childAt(event->pos()));

	if (cblw != NULL)
	{
		//colorEditorPlanLibraryWidget->graphicsEffect()->setEnabled(false);
		unhighlightAllPlanLibraryWidgets(setupDone);
		colorEditorPlanLibraryWidget = cblw;
		colorEditorPlan = cblw->getPullPlan();
		colorEditorViewWidget->setPullPlan(colorEditorPlan);
		editorStack->setCurrentIndex(COLORBAR_MODE);
		emit someDataChanged();
	}
	else if (plplw != NULL)
	{
		//pullPlanEditorPlanLibraryWidget->graphicsEffect()->setEnabled(false);
		unhighlightAllPlanLibraryWidgets(setupDone);
		pullPlanEditorPlanLibraryWidget = plplw;
		pullPlanEditorWidget->setPlan(plplw->getPullPlan());
		editorStack->setCurrentIndex(PULLPLAN_MODE);
	}
	else if (plw != NULL)
	{
		//pieceEditorPieceLibraryWidget->graphicsEffect()->setEnabled(false);
		unhighlightAllPlanLibraryWidgets(setupDone);
		pieceEditorPieceLibraryWidget = plw;
		pieceEditorPiece = plw->getPiece();
		editorStack->setCurrentIndex(PIECE_MODE);
		emit someDataChanged();
	}
	else if (ptlw != NULL)
	{
		pullPlanEditorWidget->setPlanTemplate(new PullTemplate(ptlw->getPullTemplateType()));
	}
	else if (pktlw != NULL)
	{
		if (pktlw->getPickupTemplateType() != pieceEditorPiece->pickup->getTemplate()->type)
		{
			pieceEditorPiece->pickup->setTemplate(new PickupTemplate(pktlw->getPickupTemplateType()));
			pickupPlanEditorViewWidget->setPiece(pieceEditorPiece);
			pickupTemplateParameter1Label->setText(pieceEditorPiece->pickup->getTemplate()->getParameterName(0));
			pickupTemplateParameter1SpinBox->setValue(pieceEditorPiece->pickup->getTemplate()->getParameter(0));
			emit someDataChanged();
		}
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

	ColorBarLibraryWidget* cblw = dynamic_cast<ColorBarLibraryWidget*>(childAt(event->pos()));
	PullPlanLibraryWidget* plplw = dynamic_cast<PullPlanLibraryWidget*>(childAt(event->pos()));
	PieceLibraryWidget* plw = dynamic_cast<PieceLibraryWidget*>(childAt(event->pos()));
	int type;
	if (cblw != NULL)
	{
		plan = cblw->getPullPlan();
		pixmap = *cblw->pixmap();
		type = COLOR_BAR_MIME;
	}
	else if (plplw != NULL)
	{
		plan = plplw->getPullPlan();
		pixmap = *plplw->getEditorPixmap();
		type = PULL_PLAN_MIME;
	}
	else if (plw != NULL)
	{
		plan = plw->getPiece();
		pixmap = *plw->getEditorPixmap();
		type = PIECE_MIME;
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
	connect(newColorBarButton, SIGNAL(pressed()), this, SLOT(newColorBar()));
	connect(colorEditorViewWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

	connect(newPullPlanButton, SIGNAL(pressed()), this, SLOT(newPullPlan()));
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(pullPlanEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

	connect(pickupPlanEditorViewWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(pickupTemplateParameter1SpinBox, SIGNAL(valueChanged(int)),
		this, SLOT(pickupTemplateParameter1SpinBoxChanged(int)));

	connect(pieceTemplateComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pieceTemplateComboBoxChanged(int)));
	connect(newPieceButton, SIGNAL(pressed()), this, SLOT(newPiece()));
	connect(pieceTemplateParameter1Slider, SIGNAL(valueChanged(int)),
		this, SLOT(pieceTemplateParameterSlider1Changed(int)));
	connect(pieceTemplateParameter2Slider, SIGNAL(valueChanged(int)),
		this, SLOT(pieceTemplateParameterSlider2Changed(int)));

	connect(writeRawCheckBox, SIGNAL(stateChanged(int)), this, SLOT(writeRawCheckBoxChanged(int)));
}

void MainWindow :: writeRawCheckBoxChanged(int)
{
	updatePieceEditor(); // cause geometry to be updated correctly
}

void MainWindow :: setupTable()
{
	QVBoxLayout* tableLayout = new QVBoxLayout(centralWidget);
	centralLayout->addLayout(tableLayout, 1);

	QWidget* tableGridLibraryWidget = new QWidget(centralWidget);
	tableGridLayout = new QGridLayout(tableGridLibraryWidget);
	tableGridLayout->setSpacing(10);
	tableGridLibraryWidget->setLayout(tableGridLayout);
	colorBarCount = pullPlanCount = pieceCount = 1;

	newColorBarButton = new QPushButton("New Color");
	newPullPlanButton = new QPushButton("New Cane");
	newPieceButton = new QPushButton("New Piece");
	tableGridLayout->setAlignment(Qt::AlignTop);
	tableGridLayout->addWidget(newColorBarButton, 0, 0);
	tableGridLayout->addWidget(newPullPlanButton, 0, 1);
	tableGridLayout->addWidget(newPieceButton, 0, 2);

	QScrollArea* tableGridLibraryScrollArea = new QScrollArea;
	tableGridLibraryScrollArea->setBackgroundRole(QPalette::Dark);
	tableGridLibraryScrollArea->setWidget(tableGridLibraryWidget);
	tableGridLibraryScrollArea->setWidgetResizable(true);
	tableGridLibraryScrollArea->setFixedWidth(380);
	tableGridLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	tableGridLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	tableLayout->addWidget(tableGridLibraryScrollArea);

	QLabel* descriptionLabel = new QLabel("Library - click to edit or drag into edited item.",
		centralWidget);
	descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	tableLayout->addWidget(descriptionLabel, 0);
}

void MainWindow :: setupEditors()
{
	editorStack = new QStackedWidget(centralWidget);
	centralLayout->addWidget(editorStack, 2);

	setupEmptyPaneEditor();
	editorStack->addWidget(emptyEditorPage);

	setupColorEditor();
	editorStack->addWidget(colorEditorPage);

	setupPullPlanEditor();
	editorStack->addWidget(pullPlanEditorWidget);

	setupPieceEditor();
	editorStack->addWidget(pieceEditorPage);
}

void MainWindow :: setupPieceEditor()
{
	pieceEditorPage = new QWidget(editorStack);
	QHBoxLayout* piecePageLayout = new QHBoxLayout(pieceEditorPage);
	pieceEditorPage->setLayout(piecePageLayout);
	pieceEditorPiece = new Piece(TUMBLER_TEMPLATE);

	QVBoxLayout* leftLayout = new QVBoxLayout(pieceEditorPage);
	piecePageLayout->addLayout(leftLayout);
	pickupPlanEditorViewWidget = new PickupPlanEditorViewWidget(pieceEditorPiece, model, pieceEditorPage);
	leftLayout->addWidget(pickupPlanEditorViewWidget);

	// Setup pickup template scrolling library
	QWidget* pickupTemplateLibraryWidget = new QWidget(pieceEditorPage);
	pickupTemplateLibraryLayout = new QHBoxLayout(pickupTemplateLibraryWidget);
	pickupTemplateLibraryLayout->setSpacing(10);
	pickupTemplateLibraryWidget->setLayout(pickupTemplateLibraryLayout);

	QScrollArea* pickupTemplateLibraryScrollArea = new QScrollArea(pieceEditorPage);
	pickupTemplateLibraryScrollArea->setBackgroundRole(QPalette::Dark);
	pickupTemplateLibraryScrollArea->setWidget(pickupTemplateLibraryWidget);
	pickupTemplateLibraryScrollArea->setWidgetResizable(true);
	pickupTemplateLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	pickupTemplateLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pickupTemplateLibraryScrollArea->setFixedHeight(130);
	leftLayout->addWidget(pickupTemplateLibraryScrollArea, 0);

	pickupTemplateParameter1Label = new QLabel(pieceEditorPiece->pickup->getTemplate()->getParameterName(0));
	pickupTemplateParameter1SpinBox = new QSpinBox(pieceEditorPage);
	pickupTemplateParameter1SpinBox->setRange(6, 40);
	pickupTemplateParameter1SpinBox->setSingleStep(1);
	pickupTemplateParameter1SpinBox->setValue(1);

	QHBoxLayout* parameter1Layout = new QHBoxLayout(pieceEditorPage);
	leftLayout->addLayout(parameter1Layout, 0);
	parameter1Layout->addWidget(pickupTemplateParameter1Label, 0);
	parameter1Layout->addWidget(pickupTemplateParameter1SpinBox, 0);
	parameter1Layout->addStretch(1);

	// Little description for the editor
	QLabel* descriptionLabel = new QLabel("Pickup editor - drag in canes or change templates.", pieceEditorPage);
	descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	leftLayout->addWidget(descriptionLabel, 0);

	QPixmap pixmap(100, 100);
	pixmap.fill(Qt::white);
	pieceEditorPieceLibraryWidget = new PieceLibraryWidget(pixmap, pixmap, pieceEditorPiece);
	//pieceEditorPieceLibraryWidget->setGraphicsEffect(new QGraphicsColorizeEffect());
	tableGridLayout->addWidget(pieceEditorPieceLibraryWidget, pieceCount, 2);
	++pieceCount;

	pieceTemplateComboBox = new QComboBox(pieceEditorPage);
	pieceTemplateComboBox->addItem("Tumbler");
	pieceTemplateComboBox->addItem("Bowl");
	pieceTemplateComboBox->addItem("Vase");
	leftLayout->addWidget(pieceTemplateComboBox, 0);

	pieceTemplateParameter1Label = new QLabel(pieceEditorPiece->getTemplate()->parameterNames[0]);
	pieceTemplateParameter1Slider = new QSlider(Qt::Horizontal, pieceEditorPage);
	pieceTemplateParameter1Slider->setRange(0, 100);
	pieceTemplateParameter1Slider->setTickPosition(QSlider::TicksBothSides);
	pieceTemplateParameter1Slider->setSliderPosition(0);

	QHBoxLayout* pieceParameter1Layout = new QHBoxLayout(pieceEditorPage);
	leftLayout->addLayout(pieceParameter1Layout);
	pieceParameter1Layout->addWidget(pieceTemplateParameter1Label);
	pieceParameter1Layout->addWidget(pieceTemplateParameter1Slider);

	pieceTemplateParameter2Label = new QLabel(pieceEditorPiece->getTemplate()->parameterNames[1]);
	pieceTemplateParameter2Slider = new QSlider(Qt::Horizontal, pieceEditorPage);
	pieceTemplateParameter2Slider->setRange(0, 100);
	pieceTemplateParameter2Slider->setTickPosition(QSlider::TicksBothSides);
	pieceTemplateParameter2Slider->setSliderPosition(0);

	QHBoxLayout* parameter2Layout = new QHBoxLayout(pieceEditorPage);
	leftLayout->addLayout(parameter2Layout);
	parameter2Layout->addWidget(pieceTemplateParameter2Label);
	parameter2Layout->addWidget(pieceTemplateParameter2Slider);

	// Little description for the editor
	QLabel* pieceEditorDescriptionLabel = new QLabel("Piece editor", pieceEditorPage);
	pieceEditorDescriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	leftLayout->addWidget(pieceEditorDescriptionLabel, 0);

	QVBoxLayout* niceViewLayout = new QVBoxLayout(pieceEditorPage);
	piecePageLayout->addLayout(niceViewLayout, 1);
		pieceNiceViewWidget = new NiceViewWidget(pieceEditorPage);
		niceViewLayout->addWidget(pieceNiceViewWidget, 10);

	writeRawCheckBox = new QCheckBox("Write .raw file", pieceEditorPage);
		writeRawCheckBox->setCheckState(Qt::Unchecked);
		niceViewLayout->addWidget(writeRawCheckBox, 0);

	// Little description for the editor
	QLabel* niceViewDescriptionLabel = new QLabel("3D view of piece.",
		pieceEditorPage);
	niceViewDescriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	niceViewLayout->addWidget(niceViewDescriptionLabel, 0);
}


void MainWindow :: setupEmptyPaneEditor()
{
	emptyEditorPage = new QWidget(editorStack);
	QHBoxLayout* editorLayout = new QHBoxLayout(emptyEditorPage);
	emptyEditorPage->setLayout(editorLayout);
	QLabel* whatToDoLabel = new QLabel("Click a library item at left to modify it.", emptyEditorPage);
	whatToDoLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	editorLayout->addWidget(whatToDoLabel, 0);
}

void MainWindow :: setupColorEditor()
{
	Color* color = new Color();
	color->r = color->g = color->b = color->a = 1.0;
	colorEditorPlan = new PullPlan(CIRCLE_BASE_PULL_TEMPLATE, color);
	QPixmap pixmap(100, 100);
	pixmap.fill(Qt::white);
	colorEditorPlanLibraryWidget = new ColorBarLibraryWidget(pixmap,colorEditorPlan);
	//colorEditorPlanLibraryWidget->setGraphicsEffect(new QGraphicsColorizeEffect());
	tableGridLayout->addWidget(colorEditorPlanLibraryWidget, colorBarCount, 0);
	++colorBarCount;

	colorEditorPage = new QWidget(editorStack);
	QHBoxLayout* pageLayout = new QHBoxLayout(colorEditorPage);
	colorEditorPage->setLayout(pageLayout);
	QVBoxLayout* editorLayout = new QVBoxLayout(colorEditorPage);
	pageLayout->addLayout(editorLayout);

	colorEditorViewWidget = new ColorEditorViewWidget(colorEditorPlan, colorEditorPage);
	editorLayout->addWidget(colorEditorViewWidget, 0);

	colorBarNiceViewWidget = new NiceViewWidget(colorEditorPage);
	pageLayout->addWidget(colorBarNiceViewWidget, 10);

	// Little description for the editor
	QLabel* descriptionLabel = new QLabel("Color editor", colorEditorPage);
	descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	editorLayout->addWidget(descriptionLabel, 0);
}

void MainWindow :: setupPullPlanEditor()
{
	// Setup data objects - the current plan and library widget for this plan
	pullPlanEditorWidget = new PullPlanEditorWidget(editorStack);
	pullPlanEditorPlanLibraryWidget = new PullPlanLibraryWidget(pullPlanEditorWidget->getPlan());
	pullPlanEditorWidget->updateLibraryWidgetPixmaps(pullPlanEditorPlanLibraryWidget);
	tableGridLayout->addWidget(pullPlanEditorPlanLibraryWidget, pullPlanCount, 1);
	++pullPlanCount;
}

void MainWindow :: pieceTemplateParameterSlider2Changed(int)
{
	int value = pieceTemplateParameter2Slider->sliderPosition();

	if (value == pieceEditorPiece->getTemplate()->parameterValues[1])
		return;
	pieceEditorPiece->getTemplate()->parameterValues[1] = value;
	someDataChanged();
}

void MainWindow :: pieceTemplateParameterSlider1Changed(int)
{
	int value = pieceTemplateParameter1Slider->sliderPosition();

	if (value == pieceEditorPiece->getTemplate()->parameterValues[0])
		return;
	pieceEditorPiece->getTemplate()->parameterValues[0] = value;
	someDataChanged();
}

void MainWindow :: pickupTemplateParameter1SpinBoxChanged(int)
{
	int value = pickupTemplateParameter1SpinBox->value();

	if (value == pieceEditorPiece->pickup->getTemplate()->getParameter(0))
		return;

	pieceEditorPiece->pickup->getTemplate()->setParameter(0, value);
	pieceEditorPiece->pickup->setTemplate(pieceEditorPiece->pickup->getTemplate()); // just push changes through
	someDataChanged();
}

void MainWindow :: newPiece()
{
	pieceEditorPiece = pieceEditorPiece->copy();

	unhighlightAllPlanLibraryWidgets(setupDone);
	QPixmap pixmap(100, 100);
	pixmap.fill(Qt::white);
	pieceEditorPieceLibraryWidget = new PieceLibraryWidget(pixmap, pixmap, pieceEditorPiece);
	pieceEditorPiece->setLibraryWidget(pieceEditorPieceLibraryWidget);
	tableGridLayout->addWidget(pieceEditorPieceLibraryWidget, pieceCount, 2);
	++pieceCount;

	pickupPlanEditorViewWidget->setPiece(pieceEditorPiece);

	// Load up the right editor
	editorStack->setCurrentIndex(PIECE_MODE);

	emit someDataChanged();
}

void MainWindow :: newColorBar()
{
	Color* newColor = new Color();
	newColor->r = newColor->g = newColor->b = newColor->a = 1.0;
	colorEditorPlan = new PullPlan(CIRCLE_BASE_PULL_TEMPLATE, newColor);
	//colorEditorPlanLibraryWidget->graphicsEffect()->setEnabled(false);
	unhighlightAllPlanLibraryWidgets(setupDone);
	QPixmap pixmap(100, 100);
	pixmap.fill(Qt::white);
	colorEditorPlanLibraryWidget = new ColorBarLibraryWidget(pixmap, colorEditorPlan);
	//colorEditorPlanLibraryWidget->setGraphicsEffect(new QGraphicsColorizeEffect());
	tableGridLayout->addWidget(colorEditorPlanLibraryWidget, colorBarCount, 0);
	++colorBarCount;

	// Give the new plan to the editor
	colorEditorViewWidget->setPullPlan(colorEditorPlan);

	// Load up the right editor
	editorStack->setCurrentIndex(COLORBAR_MODE);

	// Trigger GUI updates
	emit someDataChanged();
}

void MainWindow :: newPullPlan()
{
	PullPlan* oldEditorPlan = pullPlanEditorWidget->getPlan();

	// Create the new plan
	PullPlan* newEditorPlan = new PullPlan(oldEditorPlan->getTemplate()->type, oldEditorPlan->color);
	newEditorPlan->getTemplate()->setCasingThickness(oldEditorPlan->getTemplate()->getCasingThickness());
	newEditorPlan->twist = oldEditorPlan->twist;
	for (unsigned int i = 0; i < oldEditorPlan->subplans.size(); ++i)
	{
		newEditorPlan->subplans.push_back(oldEditorPlan->subplans[i]);
	}

	// Create the new library entry
	unhighlightAllPlanLibraryWidgets(setupDone);
	pullPlanEditorPlanLibraryWidget = new PullPlanLibraryWidget(pullPlanEditorWidget->getPlan());
	tableGridLayout->addWidget(pullPlanEditorPlanLibraryWidget, pullPlanCount, 1);
	++pullPlanCount;

	// Give the new plan to the editor
	pullPlanEditorWidget->setPlan(newEditorPlan);

	// Load up the right editor
	editorStack->setCurrentIndex(PULLPLAN_MODE);

	updateLibrary();
}

void MainWindow :: highlightPlanLibraryWidgets(ColorBarLibraryWidget* cblw,bool highlight,bool setupDone) {

	if (!setupDone)
		return;

	if (cblw==NULL || cblw->graphicsEffect()==NULL)
		return;

	if (cblw->graphicsEffect()->isEnabled() == highlight)
		return;

	if (cblw == colorEditorPlanLibraryWidget && editorStack->currentIndex() == COLORBAR_MODE)
		((QGraphicsHighlightEffect*) cblw->graphicsEffect())->setActiveMain(true);
	else
		((QGraphicsHighlightEffect*) cblw->graphicsEffect())->setActiveMain(false);

	cblw->graphicsEffect()->setEnabled(highlight);

	if (!setupDone)
		return;

	if (!cblw->getPullPlan()->subplans.empty())
	{
		for (unsigned int j = 0; j < cblw->getPullPlan()->subplans.size(); j++)
		{
			if (cblw->getPullPlan()->subplans[j])
				highlightPlanLibraryWidgets(cblw->getPullPlan()->subplans[j]->getColorLibraryWidget(),highlight,true);
		}
	}
}


void MainWindow :: highlightPlanLibraryWidgets(PullPlanLibraryWidget* plplw,bool highlight,bool setupDone) {
	if (!plplw || !plplw->graphicsEffect())
		return;
	if (plplw->graphicsEffect()->isEnabled() == highlight)
		return;

	if (plplw == pullPlanEditorPlanLibraryWidget && editorStack->currentIndex() == PULLPLAN_MODE)
		((QGraphicsHighlightEffect*) plplw->graphicsEffect())->setActiveMain(true);
	else
		((QGraphicsHighlightEffect*) plplw->graphicsEffect())->setActiveMain(false);

	plplw->graphicsEffect()->setEnabled(highlight);

	if (!setupDone)
		return;

	if (plplw->getPullPlan()->subplans.empty())
		return;

	for (unsigned int j = 0; j < plplw->getPullPlan()->subplans.size(); j++)
	{
		if (plplw->getPullPlan()->subplans[j])
		{
			highlightPlanLibraryWidgets(plplw->getPullPlan()->subplans.at(j)->getColorLibraryWidget(),highlight,true);
			highlightPlanLibraryWidgets(plplw->getPullPlan()->subplans.at(j)->getLibraryWidget(),highlight,true);
		}
	}
}

void MainWindow :: highlightPlanLibraryWidgets(PieceLibraryWidget* plw,bool highlight,bool setupDone) {

	if (!plw || !plw->graphicsEffect())
		return;

	if (plw->graphicsEffect()->isEnabled()==highlight)
		return;
	if (plw == pieceEditorPieceLibraryWidget && editorStack->currentIndex() == PIECE_MODE)
		((QGraphicsHighlightEffect*) plw->graphicsEffect())->setActiveMain(true);
	else
		((QGraphicsHighlightEffect*) plw->graphicsEffect())->setActiveMain(false);

	plw->graphicsEffect()->setEnabled(highlight);

	if (!setupDone)
		return;

	highlightPlanLibraryWidgets(plw->getPiece()->getLibraryWidget(),highlight,true);
}

void MainWindow :: updateEverything()
{
	switch (editorStack->currentIndex())
	{
		case COLORBAR_MODE:
			updateColorEditor();
			break;
		case PIECE_MODE:
			updatePieceEditor();
			break;
		default:
			return;
	}

	updateLibrary();
}

void MainWindow :: updateLibrary()
{
	//highlightPlanLibraryWidgets(colorEditorPlanLibraryWidget,false,setupDone);
	//highlightPlanLibraryWidgets(pullPlanEditorPlanLibraryWidget,false,setupDone);
	//highlightPlanLibraryWidgets(pieceEditorPieceLibraryWidget,false,setupDone);
	unhighlightAllPlanLibraryWidgets(setupDone);

	colorEditorPlanLibraryWidget->graphicsEffect()->setEnabled(false);
	pullPlanEditorPlanLibraryWidget->graphicsEffect()->setEnabled(false);
	pieceEditorPieceLibraryWidget->graphicsEffect()->setEnabled(false);

	switch (editorStack->currentIndex())
	{
		case COLORBAR_MODE:
			colorEditorPlanLibraryWidget->updatePixmap(
				QPixmap::fromImage(colorBarNiceViewWidget->renderImage()).scaled(100, 100));
			highlightPlanLibraryWidgets(colorEditorPlanLibraryWidget,true,setupDone);
			break;
		case PULLPLAN_MODE:
			pullPlanEditorWidget->updateLibraryWidgetPixmaps(pullPlanEditorPlanLibraryWidget);
			highlightPlanLibraryWidgets(pullPlanEditorPlanLibraryWidget,true,setupDone);
			break;
		case PIECE_MODE:
			pieceEditorPieceLibraryWidget->updatePixmaps(
				QPixmap::fromImage(pieceNiceViewWidget->renderImage()).scaled(100, 100),
				QPixmap::fromImage(pieceNiceViewWidget->renderImage()).scaled(100, 100));
			highlightPlanLibraryWidgets(pieceEditorPieceLibraryWidget,true,setupDone);
			//pieceEditorPieceLibraryWidget->graphicsEffect()->setEnabled(true);
			break;
	}
}

void MainWindow :: updatePieceEditor()
{
	// update pickup stuff
	int value = pieceEditorPiece->pickup->getTemplate()->getParameter(0);
	pickupTemplateParameter1SpinBox->setValue(value);
	pickupPlanEditorViewWidget->setPiece(pieceEditorPiece);

	// update piece stuff
	Geometry* geometry = model->getGeometry(pieceEditorPiece);
	pieceNiceViewWidget->setCameraMode(PIECE_MODE);
	pieceNiceViewWidget->setGeometry(geometry);
	if (writeRawCheckBox->checkState() == Qt::Checked)
		geometry->save_raw_file("./cane.raw");
	pieceTemplateComboBox->setCurrentIndex(pieceEditorPiece->getTemplate()->type-1);
	pieceTemplateParameter1Label->setText(pieceEditorPiece->getTemplate()->parameterNames[0]);
	pieceTemplateParameter1Slider->setSliderPosition(pieceEditorPiece->getTemplate()->parameterValues[0]);
	pieceTemplateParameter2Label->setText(pieceEditorPiece->getTemplate()->parameterNames[1]);
	pieceTemplateParameter2Slider->setSliderPosition(pieceEditorPiece->getTemplate()->parameterValues[1]);
}

void MainWindow :: updateColorEditor()
{
	Geometry* geometry = model->getGeometry(colorEditorPlan);
	colorBarNiceViewWidget->setCameraMode(PULLPLAN_MODE);
	colorBarNiceViewWidget->setGeometry(geometry);
	if (writeRawCheckBox->checkState() == Qt::Checked)
		geometry->save_raw_file("./cane.raw");
}

void MainWindow :: pieceTemplateComboBoxChanged(int newIndex)
{
	if (newIndex+1 != pieceEditorPiece->getTemplate()->type)
	{
		pieceEditorPiece->setTemplate(new PieceTemplate(newIndex+1));
		pieceTemplateParameter1Label->setText(pieceEditorPiece->getTemplate()->parameterNames[0]);
		pieceTemplateParameter2Label->setText(pieceEditorPiece->getTemplate()->parameterNames[1]);
		emit someDataChanged();
	}
}



