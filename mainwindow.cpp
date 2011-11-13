

#include "mainwindow.h"

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
	// Sampling of Reichenbach colors from Kim's color file
	Color color;
	ColorBarLibraryWidget* cblw;

	color.r = color.g = color.b = 1.0;
	color.a = 0.4;
	cblw = new ColorBarLibraryWidget(color);
	tableGridLayout->addWidget(cblw, colorBarCount, 0);
	++colorBarCount;

	color.r = 1/255.0;
	color.g = 58/255.0;
	color.b = 186/255.0;
	color.a = 128/255.0;
	cblw = new ColorBarLibraryWidget(color);
	tableGridLayout->addWidget(cblw, colorBarCount, 0);
	++colorBarCount;

	color.r = 222/255.0;
	color.g = 205/255.0;
	color.b = 1/255.0;
	color.a = 126/255.0;
	cblw = new ColorBarLibraryWidget(color);
	tableGridLayout->addWidget(cblw, colorBarCount, 0);
	++colorBarCount;

	color.r = 2/255.0;
	color.g = 101/255.0;
	color.b = 35/255.0;
	color.a = 128/255.0;
	cblw = new ColorBarLibraryWidget(color);
	tableGridLayout->addWidget(cblw, colorBarCount, 0);
	++colorBarCount;

	color.r = 253/255.0;
	color.g = 122/255.0;
	color.b = 56/255.0;
	color.a = 128/255.0;
	cblw = new ColorBarLibraryWidget(color);
	tableGridLayout->addWidget(cblw, colorBarCount, 0);
	++colorBarCount;

	color.r = 226/255.0;
	color.g = 190/255.0;
	color.b = 161/255.0;
	color.a = 255/255.0;
	cblw = new ColorBarLibraryWidget(color);
	tableGridLayout->addWidget(cblw, colorBarCount, 0);
	++colorBarCount;

	color.r = 50/255.0;
	color.g = 102/255.0;
	color.b = 54/255.0;
	color.a = 255/255.0;
	cblw = new ColorBarLibraryWidget(color);
	tableGridLayout->addWidget(cblw, colorBarCount, 0);
	++colorBarCount;

	color.r = 60/255.0;
	color.g = 31/255.0;
	color.b = 37/255.0;
	color.a = 255/255.0;
	cblw = new ColorBarLibraryWidget(color);
	tableGridLayout->addWidget(cblw, colorBarCount, 0);
	++colorBarCount;

	color.r = 131/255.0;
	color.g = 149/255.0;
	color.b = 201/255.0;
	color.a = 255/255.0;
	cblw = new ColorBarLibraryWidget(color);
	tableGridLayout->addWidget(cblw, colorBarCount, 0);
	++colorBarCount;

	color.r = 138/255.0;
	color.g = 155/255.0;
	color.b = 163/255.0;
	color.a = 255/255.0;
	cblw = new ColorBarLibraryWidget(color);
	tableGridLayout->addWidget(cblw, colorBarCount, 0);
	++colorBarCount;

	color.r = 255/255.0;
	color.g = 255/255.0;
	color.b = 255/255.0;
	color.a = 10/255.0;
	cblw = new ColorBarLibraryWidget(color);
	tableGridLayout->addWidget(cblw, colorBarCount, 0);
	++colorBarCount;

	// setup the editor/3D view
	emit someDataChanged();

	editorStack->setCurrentIndex(PULLPLAN_MODE);
	emit someDataChanged();

	// Load pull template types
	for (int i = LINE_THREE_CIRCLES_TEMPLATE; i <= SQUARE_SIXTEEN_SQUARES_TEMPLATE; ++i)
	{
		pullPlanEditorPlan->setTemplate(new PullTemplate(i, 0.0));
		pullPlanEditorPlan->getTemplate()->shape = AMORPHOUS_SHAPE; // stop the border from being drawn
		emit someDataChanged();
		PullTemplateLibraryWidget *ptlw = new PullTemplateLibraryWidget(
			QPixmap::grabWidget(pullPlanEditorViewWidget).scaled(100, 100), i);
		pullTemplateLibraryLayout->addWidget(ptlw);
	}

	// Load final starting pull plan
	pullPlanEditorPlan->setTemplate(new PullTemplate(LINE_THREE_CIRCLES_TEMPLATE, 0.0));
	emit someDataChanged();

	// Load pickup template types
	editorStack->setCurrentIndex(PIECE_MODE);
	emit someDataChanged();

	for (int i = VERTICALS_TEMPLATE; i <= MURRINE_SQUARE_TEMPLATE; ++i)
	{
		pickupPlanEditorPlan->setTemplate(new PickupTemplate(i));
		emit someDataChanged();
		PickupTemplateLibraryWidget *ptlw = new PickupTemplateLibraryWidget(
			QPixmap::grabWidget(pickupPlanEditorViewWidget).scaled(100, 100), i);
		pickupTemplateLibraryLayout->addWidget(ptlw);
	}

	// Load final starting pickup plan
	pickupPlanEditorPlan->setTemplate(new PickupTemplate(VERTICALS_TEMPLATE));
	for (unsigned int j = 0; j < pickupPlanEditorPlan->getTemplate()->subpulls.size(); ++j)
	{
		pickupPlanEditorPlan->subplans[j] = pullPlanEditorPlan;
	}
	emit someDataChanged();

	// Load correct picture of piece
	pieceEditorPlan->pickup = pickupPlanEditorPlan;
	editorStack->setCurrentIndex(PIECE_MODE);
	emit someDataChanged();

	editorStack->setCurrentIndex(EMPTY_MODE); // end in pull plan mode
	emit someDataChanged();

	setupDone = true;
}


void MainWindow :: mouseReleaseEvent(QMouseEvent* event)
{
	if (!((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance()))
	{
		return; 
	}

	PullPlanLibraryWidget* plplw = dynamic_cast<PullPlanLibraryWidget*>(childAt(event->pos()));
	PieceLibraryWidget* plw = dynamic_cast<PieceLibraryWidget*>(childAt(event->pos()));
	PullTemplateLibraryWidget* ptlw = dynamic_cast<PullTemplateLibraryWidget*>(childAt(event->pos()));
	PickupTemplateLibraryWidget* pktlw = dynamic_cast<PickupTemplateLibraryWidget*>(childAt(event->pos()));

	if (plplw != NULL)
	{
		pullPlanEditorPlanLibraryWidget->graphicsEffect()->setEnabled(false);
		pullPlanEditorPlanLibraryWidget = plplw;
		pullPlanEditorPlan = plplw->getPullPlan();
		pullPlanEditorViewWidget->setPullPlan(pullPlanEditorPlan);
		editorStack->setCurrentIndex(PULLPLAN_MODE);
		emit someDataChanged();
	}
	else if (plw != NULL)
	{
		pieceEditorPlanLibraryWidget->graphicsEffect()->setEnabled(false);
		pieceEditorPlanLibraryWidget = plw;
		pieceEditorPlan = plw->getPiece();
		editorStack->setCurrentIndex(PIECE_MODE);
		emit someDataChanged();
	}
	else if (ptlw != NULL)
	{
		pullPlanEditorPlan->setTemplate(new PullTemplate(ptlw->getPullTemplateType(), 0.0));
		emit someDataChanged();
	}
	else if (pktlw != NULL)
	{
		if (pktlw->getPickupTemplateType() != pickupPlanEditorPlan->getTemplate()->type)
		{
			pickupPlanEditorPlan->setTemplate(new PickupTemplate(pktlw->getPickupTemplateType()));
			pickupPlanEditorViewWidget->setPickupPlan(pickupPlanEditorPlan);
			pickupTemplateParameter1Label->setText(pickupPlanEditorPlan->getTemplate()->getParameterName(0));
			pickupTemplateParameter1Slider->setSliderPosition(pickupPlanEditorPlan->getTemplate()->getParameter(0));
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
	PickupPlanLibraryWidget* pkplw = dynamic_cast<PickupPlanLibraryWidget*>(childAt(event->pos()));
	PieceLibraryWidget* plw = dynamic_cast<PieceLibraryWidget*>(childAt(event->pos()));
	int type;
	if (cblw != NULL)
	{
		plan = cblw->getPullPlan();
		pixmap = *cblw->pixmap();
		type = PULL_PLAN_MIME;
	}
	else if (plplw != NULL)
	{
		plan = plplw->getPullPlan();
		pixmap = *plplw->getEditorPixmap();
		type = PULL_PLAN_MIME;
	}
	else if (pkplw != NULL)
	{
		plan = pkplw->getPickupPlan();
		pixmap = *pkplw->getEditorPixmap();
		type = PICKUP_PLAN_MIME;
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
	connect(pullTemplateShapeButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(pullTemplateShapeButtonGroupChanged(int)));
	connect(newPullPlanButton, SIGNAL(pressed()), this, SLOT(newPullPlan()));
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(pullPlanEditorViewWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(pullTemplateCasingThicknessSlider, SIGNAL(valueChanged(int)),
		this, SLOT(pullTemplateCasingThicknessSliderChanged(int)));
	connect(pullPlanTwistSlider, SIGNAL(valueChanged(int)), this, SLOT(pullPlanTwistSliderChanged(int)));
	connect(pullPlanTwistSpin, SIGNAL(valueChanged(int)), this, SLOT(pullPlanTwistSpinChanged(int)));

	connect(pickupPlanEditorViewWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(pickupTemplateParameter1Slider, SIGNAL(valueChanged(int)),
		this, SLOT(pickupTemplateParameterSlider1Changed(int)));

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
	centralLayout->addLayout(tableLayout);

	QWidget* tableGridLibraryWidget = new QWidget(centralWidget);
	tableGridLayout = new QGridLayout(tableGridLibraryWidget);
	tableGridLayout->setSpacing(10);
	tableGridLibraryWidget->setLayout(tableGridLayout);
	colorBarCount = pullPlanCount = pieceCount = 1;

	newColorButton = new QPushButton("New Color");
	newPullPlanButton = new QPushButton("New Cane");
	newPieceButton = new QPushButton("New Piece");
	tableGridLayout->addWidget(newColorButton, 0, 0);
	tableGridLayout->addWidget(newPullPlanButton, 0, 1);
	tableGridLayout->addWidget(newPieceButton, 0, 2);

	QScrollArea* tableGridLibraryScrollArea = new QScrollArea;
	tableGridLibraryScrollArea->setBackgroundRole(QPalette::Dark);
	tableGridLibraryScrollArea->setWidget(tableGridLibraryWidget);
	tableGridLibraryScrollArea->setWidgetResizable(true);
	tableGridLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	tableGridLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	tableLayout->addWidget(tableGridLibraryScrollArea);

	QLabel* descriptionLabel = new QLabel("Library - click an item to edit or drag into item currently being edited", 
		centralWidget);
	descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	tableLayout->addWidget(descriptionLabel, 0);
}

void MainWindow :: setupEditors()
{
	defaultColor.r = defaultColor.g = defaultColor.b = 1.0;
	defaultColor.a = 0.4;

	editorStack = new QStackedWidget(centralWidget);
	centralLayout->addWidget(editorStack);

	setupEmptyPaneEditor();
	editorStack->addWidget(emptyEditorPage);

	setupPullPlanEditor();
	editorStack->addWidget(pullPlanEditorPage);

	setupPieceEditor();
	editorStack->addWidget(pieceEditorPage);
}

void MainWindow :: setupPieceEditor()
{
	pieceEditorPlan = new Piece(TUMBLER_TEMPLATE);

	pieceEditorPlanLibraryWidget = new PieceLibraryWidget(QPixmap::fromImage(QImage("./duck.jpg")),
		QPixmap::fromImage(QImage("./duck.jpg")), pieceEditorPlan);
	tableGridLayout->addWidget(pieceEditorPlanLibraryWidget, pieceCount, 2);
	++pieceCount;

	pieceEditorPage = new QWidget(editorStack);

	QHBoxLayout* piecePageLayout = new QHBoxLayout(pieceEditorPage);
	pieceEditorPage->setLayout(piecePageLayout);
	QVBoxLayout* pickupPlanEditorLayout = new QVBoxLayout(pieceEditorPage);
	piecePageLayout->addLayout(pickupPlanEditorLayout);
	QVBoxLayout* pieceEditorLayout = new QVBoxLayout(pieceEditorPage);
	piecePageLayout->addLayout(pieceEditorLayout);


        pieceNiceViewWidget = new NiceViewWidget(pieceEditorPage);
        pieceEditorLayout->addWidget(pieceNiceViewWidget, 10);

        writeRawCheckBox = new QCheckBox("Write .raw file", pieceEditorPage);
        writeRawCheckBox->setCheckState(Qt::Unchecked);
        pieceEditorLayout->addWidget(writeRawCheckBox, 0);

	pieceTemplateComboBox = new QComboBox(pieceEditorPage);
	pieceTemplateComboBox->addItem("Tumbler");
	pieceTemplateComboBox->addItem("Bowl");
	pieceEditorLayout->addWidget(pieceTemplateComboBox, 0);

	pieceTemplateParameter1Label = new QLabel(pieceEditorPlan->getTemplate()->parameterNames[0]);
	pieceTemplateParameter1Slider = new QSlider(Qt::Horizontal, pieceEditorPage);
	pieceTemplateParameter1Slider->setRange(0, 100);
	pieceTemplateParameter1Slider->setTickPosition(QSlider::TicksBothSides);
	pieceTemplateParameter1Slider->setSliderPosition(0);

	QHBoxLayout* parameter1Layout = new QHBoxLayout(pieceEditorPage);
	pieceEditorLayout->addLayout(parameter1Layout);
	parameter1Layout->addWidget(pieceTemplateParameter1Label);
	parameter1Layout->addWidget(pieceTemplateParameter1Slider);

	pieceTemplateParameter2Label = new QLabel(pieceEditorPlan->getTemplate()->parameterNames[1]);
	pieceTemplateParameter2Slider = new QSlider(Qt::Horizontal, pieceEditorPage);
	pieceTemplateParameter2Slider->setRange(0, 100);
	pieceTemplateParameter2Slider->setTickPosition(QSlider::TicksBothSides);
	pieceTemplateParameter2Slider->setSliderPosition(0);

	QHBoxLayout* parameter2Layout = new QHBoxLayout(pieceEditorPage);
	pieceEditorLayout->addLayout(parameter2Layout);
	parameter2Layout->addWidget(pieceTemplateParameter2Label);
	parameter2Layout->addWidget(pieceTemplateParameter2Slider);

	// Little description for the editor
	QLabel* descriptionLabel = new QLabel("Piece editor - drag a pickup into the piece", pieceEditorPage);
	descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	pieceEditorLayout->addWidget(descriptionLabel, 0);	

	pickupPlanEditorPlan = new PickupPlan(VERTICALS_TEMPLATE);

	pickupPlanEditorViewWidget = new PickupPlanEditorViewWidget(pickupPlanEditorPlan, pieceEditorPage);
	pickupPlanEditorLayout->addWidget(pickupPlanEditorViewWidget, 10);

	// Setup pickup template scrolling library
	QWidget* pickupTemplateLibraryWidget = new QWidget(centralWidget);
	pickupTemplateLibraryLayout = new QHBoxLayout(pickupTemplateLibraryWidget);
	pickupTemplateLibraryLayout->setSpacing(10);
	pickupTemplateLibraryWidget->setLayout(pullTemplateLibraryLayout);

	QScrollArea* pickupTemplateLibraryScrollArea = new QScrollArea(centralWidget);
	pickupTemplateLibraryScrollArea->setBackgroundRole(QPalette::Dark);
	pickupTemplateLibraryScrollArea->setWidget(pickupTemplateLibraryWidget);
	pickupTemplateLibraryScrollArea->setWidgetResizable(true);
	pickupTemplateLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	pickupTemplateLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pickupTemplateLibraryScrollArea->setFixedHeight(130);
	pickupTemplateLibraryScrollArea->setFixedWidth(520);
	pickupPlanEditorLayout->addWidget(pickupTemplateLibraryScrollArea);

	pickupTemplateParameter1Label = new QLabel(pickupPlanEditorPlan->getTemplate()->getParameterName(0));
	pickupTemplateParameter1Slider = new QSlider(Qt::Horizontal, pieceEditorPage);
	pickupTemplateParameter1Slider->setRange(0, 100);
	pickupTemplateParameter1Slider->setTickPosition(QSlider::TicksBothSides);
	pickupTemplateParameter1Slider->setSliderPosition(0);

	parameter1Layout = new QHBoxLayout(pieceEditorPage);
	pickupPlanEditorLayout->addLayout(parameter1Layout);
	parameter1Layout->addWidget(pickupTemplateParameter1Label);
	parameter1Layout->addWidget(pickupTemplateParameter1Slider);

	// Little description for the editor
	descriptionLabel = new QLabel("Pickup editor - drag color or canes into the pickup\nor select a new template above", pieceEditorPage);
	descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	pickupPlanEditorLayout->addWidget(descriptionLabel, 0);	
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

void MainWindow :: setupPullPlanEditor()
{
	// Setup data objects - the current plan and library widget for this plan
	pullPlanEditorPlan = new PullPlan(LINE_THREE_CIRCLES_TEMPLATE, false, defaultColor);

	pullPlanEditorPlanLibraryWidget = new PullPlanLibraryWidget(QPixmap::fromImage(QImage("./duck.jpg")), QPixmap::fromImage(QImage("./duck.jpg")), pullPlanEditorPlan);
	tableGridLayout->addWidget(pullPlanEditorPlanLibraryWidget, pullPlanCount, 1);
	++pullPlanCount;

	// Setup the editor layout 
	pullPlanEditorPage = new QWidget(editorStack);
	QVBoxLayout* editorLayout = new QVBoxLayout(pullPlanEditorPage);
	pullPlanEditorPage->setLayout(editorLayout);

	pullPlanEditorViewWidget = new PullPlanEditorViewWidget(pullPlanEditorPlan, pullPlanEditorPage);
	editorLayout->addWidget(pullPlanEditorViewWidget, 10);

	// Setup pull template scrolling library
	QWidget* pullTemplateLibraryWidget = new QWidget(centralWidget);
	pullTemplateLibraryLayout = new QHBoxLayout(pullTemplateLibraryWidget);
	pullTemplateLibraryLayout->setSpacing(10);
	pullTemplateLibraryWidget->setLayout(pullTemplateLibraryLayout);

	QScrollArea* pullTemplateLibraryScrollArea = new QScrollArea(centralWidget);
	pullTemplateLibraryScrollArea->setBackgroundRole(QPalette::Dark);
	pullTemplateLibraryScrollArea->setWidget(pullTemplateLibraryWidget);
	pullTemplateLibraryScrollArea->setWidgetResizable(true);
	pullTemplateLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	pullTemplateLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pullTemplateLibraryScrollArea->setFixedHeight(130);
	pullTemplateLibraryScrollArea->setFixedWidth(520);
	editorLayout->addWidget(pullTemplateLibraryScrollArea);


	QCheckBox* circleCheckBox = new QCheckBox("Circle");
	QCheckBox* squareCheckBox = new QCheckBox("Square");
	pullTemplateShapeButtonGroup = new QButtonGroup();
	pullTemplateShapeButtonGroup->addButton(circleCheckBox, 1);
	pullTemplateShapeButtonGroup->addButton(squareCheckBox, 2);
	QHBoxLayout* pullTemplateShapeLayout = new QHBoxLayout(pullPlanEditorPage);
	pullTemplateShapeLayout->addWidget(circleCheckBox);
	pullTemplateShapeLayout->addWidget(squareCheckBox);
	editorLayout->addLayout(pullTemplateShapeLayout);

	// Casing thickness slider stuff
	QHBoxLayout* casingThicknessLayout = new QHBoxLayout(pullPlanEditorPage);
	editorLayout->addLayout(casingThicknessLayout);

	QLabel* casingLabel1 = new QLabel("Casing Thickness:", pullPlanEditorPage);
	casingThicknessLayout->addWidget(casingLabel1, 0);

	QLabel* casingLabel2 = new QLabel("0%", pullPlanEditorPage);
	casingThicknessLayout->addWidget(casingLabel2, 0);

	pullTemplateCasingThicknessSlider = new QSlider(Qt::Horizontal, pullPlanEditorPage);
	pullTemplateCasingThicknessSlider->setRange(0, 100);
	//pullTemplateCasingThicknessSlider->setTickInterval(0.1);
	pullTemplateCasingThicknessSlider->setTickPosition(QSlider::TicksBothSides);
	pullTemplateCasingThicknessSlider->setSliderPosition(0);
	casingThicknessLayout->addWidget(pullTemplateCasingThicknessSlider, 10);

	QLabel* casingLabel3 = new QLabel("100%", pullPlanEditorPage);
	casingThicknessLayout->addWidget(casingLabel3, 0);

	// Twist slider stuff
	QHBoxLayout* twistLayout = new QHBoxLayout(pullPlanEditorPage);
	editorLayout->addLayout(twistLayout);

	QLabel* twistLabel1 = new QLabel("Twist:", pullPlanEditorPage);
	twistLayout->addWidget(twistLabel1);

	pullPlanTwistSpin = new QSpinBox(pullPlanEditorPage);
	pullPlanTwistSpin->setRange(-50, 50);
	pullPlanTwistSpin->setSingleStep(1);
	twistLayout->addWidget(pullPlanTwistSpin, 1);

	QLabel* twistLabel2 = new QLabel("-50", pullPlanEditorPage);
	twistLayout->addWidget(twistLabel2);

	pullPlanTwistSlider = new QSlider(Qt::Horizontal, pullPlanEditorPage);
	pullPlanTwistSlider->setRange(-50, 50);
	pullPlanTwistSlider->setTickInterval(5);
	pullPlanTwistSlider->setTickPosition(QSlider::TicksBothSides);
	pullPlanTwistSlider->setSliderPosition(0);
	twistLayout->addWidget(pullPlanTwistSlider, 10);

	QLabel* twistLabel3 = new QLabel("50", pullPlanEditorPage);
	twistLayout->addWidget(twistLabel3);

	// Little description for the editor
	QLabel* descriptionLabel = new QLabel("Cane editor - drag color or other canes into the cane to edit", 
		pullPlanEditorPage);
	descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	editorLayout->addWidget(descriptionLabel, 0);	
}

void MainWindow :: pullTemplateShapeButtonGroupChanged(int)
{
	switch (pullTemplateShapeButtonGroup->checkedId())
	{
		case 1:
			if (pullPlanEditorPlan->getTemplate()->shape == CIRCLE_SHAPE)
				return;
			pullPlanEditorPlan->getTemplate()->shape = CIRCLE_SHAPE;
			someDataChanged();
			break;
		case 2:
			if (pullPlanEditorPlan->getTemplate()->shape == SQUARE_SHAPE)
				return;
			pullPlanEditorPlan->getTemplate()->shape = SQUARE_SHAPE;
			someDataChanged();
			break;
	}
}

void MainWindow :: pullPlanTwistSpinChanged(int)
{
	int tick = pullPlanTwistSpin->value();
	pullPlanTwistSlider->setSliderPosition(tick);
	someDataChanged();
}

void MainWindow :: pieceTemplateParameterSlider2Changed(int)
{
	int value = pieceTemplateParameter2Slider->sliderPosition();
	pieceEditorPlan->getTemplate()->parameterValues[1] = value;
	someDataChanged();
}

void MainWindow :: pieceTemplateParameterSlider1Changed(int)
{
	int value = pieceTemplateParameter1Slider->sliderPosition();
	pieceEditorPlan->getTemplate()->parameterValues[0] = value;
	someDataChanged();
}

void MainWindow :: pickupTemplateParameterSlider1Changed(int)
{
	int value = pickupTemplateParameter1Slider->sliderPosition();
	pickupPlanEditorPlan->getTemplate()->setParameter(0, value);
	pickupPlanEditorPlan->setTemplate(pickupPlanEditorPlan->getTemplate()); // just push changes through
	someDataChanged();
}

void MainWindow :: pullTemplateCasingThicknessSliderChanged(int)
{
	float thickness = pullTemplateCasingThicknessSlider->sliderPosition() / 100.0;
	pullPlanEditorPlan->getTemplate()->setCasingThickness(thickness);
	someDataChanged();
}

void MainWindow :: pullPlanTwistSliderChanged(int)
{
	float twist = pullPlanTwistSlider->sliderPosition();
	pullPlanEditorPlan->twist = twist;
	someDataChanged();
}

void MainWindow :: newPiece()
{
	pieceEditorPlan = new Piece(TUMBLER_TEMPLATE);
	pickupPlanEditorPlan = new PickupPlan(VERTICALS_TEMPLATE);

	pieceEditorPlanLibraryWidget->graphicsEffect()->setEnabled(false);
	pieceEditorPlanLibraryWidget = new PieceLibraryWidget(QPixmap::fromImage(QImage("./duck.jpg")),
		QPixmap::fromImage(QImage("./duck.jpg")), pieceEditorPlan);
	tableGridLayout->addWidget(pieceEditorPlanLibraryWidget, pieceCount, 2);
	++pieceCount;

	pickupPlanEditorViewWidget->setPickupPlan(pickupPlanEditorPlan);

	// Load up the right editor
	editorStack->setCurrentIndex(PIECE_MODE);

	emit someDataChanged();
}

void MainWindow :: newPullPlan()
{
	// Create the new plan
	pullPlanEditorPlan = new PullPlan(LINE_THREE_CIRCLES_TEMPLATE, false, defaultColor);

	// Create the new library entry
	pullPlanEditorPlanLibraryWidget->graphicsEffect()->setEnabled(false);

	pullPlanEditorPlanLibraryWidget = new PullPlanLibraryWidget(QPixmap::fromImage(QImage("./duck.jpg")),
		QPixmap::fromImage(QImage("./duck.jpg")), pullPlanEditorPlan);
	tableGridLayout->addWidget(pullPlanEditorPlanLibraryWidget, pullPlanCount, 1);
	++pullPlanCount;

	// Give the new plan to the editor
	pullPlanEditorViewWidget->setPullPlan(pullPlanEditorPlan);

	// Load up the right editor
	editorStack->setCurrentIndex(PULLPLAN_MODE);

	// Trigger GUI updates
	emit someDataChanged();
}

void MainWindow :: highlightPlanLibraryWidgets(PullPlanLibraryWidget* plplw,bool highlight,bool setupDone) {

	if (!plplw || !plplw->graphicsEffect())
		return;

	if (plplw->graphicsEffect()->isEnabled() == highlight)
		return;

	plplw->graphicsEffect()->setEnabled(highlight);

	if (!setupDone)
		return;

	if (plplw->getPullPlan()->subplans.empty())
		return;

	for (unsigned int j = 0; j < plplw->getPullPlan()->subplans.size(); j++) 
	{
		if (plplw->getPullPlan()->subplans[j])
			highlightPlanLibraryWidgets(plplw->getPullPlan()->subplans[j]->getLibraryWidget(),highlight,true);
	}
}

void MainWindow :: highlightPlanLibraryWidgets(PickupPlanLibraryWidget* pkplw,bool highlight,bool setupDone) {

	if (!pkplw || !pkplw->graphicsEffect())
		return;

	pkplw->graphicsEffect()->setEnabled(highlight);

	if (!setupDone)
		return;

	if (pkplw->getPickupPlan()->subplans.empty())
		return;

	for (unsigned int j = 0; j < pkplw->getPickupPlan()->subplans.size(); j++) 
	{
		if (pkplw->getPickupPlan()->subplans[j])
			highlightPlanLibraryWidgets(pkplw->getPickupPlan()->subplans[j]->getLibraryWidget(),highlight,true);
	}
}

void MainWindow :: highlightPlanLibraryWidgets(PieceLibraryWidget* plw,bool highlight,bool setupDone) {

	if (!plw || !plw->graphicsEffect())
		return;

	if (plw->graphicsEffect()->isEnabled()==highlight)
		return;

	plw->graphicsEffect()->setEnabled(highlight);

	if (!setupDone)
		return;

	highlightPlanLibraryWidgets(plw->getPiece()->getLibraryWidget(),highlight,true);
}

void MainWindow :: updateEverything()
{
	updateLibrary();

        switch (editorStack->currentIndex())
        {
                case PULLPLAN_MODE:
			updatePullPlanEditor();
                        break;
                case PIECE_MODE:
			updatePieceEditor();
                        break;
                default:
                        return;
        }

}

void MainWindow :: updateLibrary()
{
	pullPlanEditorPlanLibraryWidget->graphicsEffect()->setEnabled(false);
	pieceEditorPlanLibraryWidget->graphicsEffect()->setEnabled(false);

	switch (editorStack->currentIndex())
	{
		case PULLPLAN_MODE:
			pullPlanEditorPlanLibraryWidget->updatePixmaps(
				QPixmap::grabWidget(pullPlanEditorViewWidget).scaled(100, 100), //niceViewWidget->renderImage()).scaled(100, 100),
				QPixmap::grabWidget(pullPlanEditorViewWidget).scaled(100, 100));
			pullPlanEditorPlanLibraryWidget->graphicsEffect()->setEnabled(true);
			break;
		case PIECE_MODE:
			pieceEditorPlanLibraryWidget->updatePixmaps(
				QPixmap::fromImage(pieceNiceViewWidget->renderImage()).scaled(100, 100),
				QPixmap::fromImage(pieceNiceViewWidget->renderImage()).scaled(100, 100));
			pieceEditorPlanLibraryWidget->graphicsEffect()->setEnabled(true);
			break;
	}
}

void MainWindow :: updatePieceEditor()
{
	// update pickup stuff
	pickupPlanEditorViewWidget->repaint();

	// update piece stuff
        Geometry* geometry = model->getGeometry(pieceEditorPlan);
	pieceNiceViewWidget->setCameraMode(PIECE_MODE);
        pieceNiceViewWidget->setGeometry(geometry);
        if (writeRawCheckBox->checkState() == Qt::Checked)
                geometry->save_raw_file("./cane.raw");

	pieceTemplateComboBox->setCurrentIndex(pieceEditorPlan->getTemplate()->type-1);
}

void MainWindow :: updatePullPlanEditor()
{
	// Only attempt to set the shape if it's defined; it's undefined during loading
	if (pullPlanEditorPlan->getTemplate()->shape < AMORPHOUS_SHAPE)
		static_cast<QCheckBox*>(pullTemplateShapeButtonGroup->button(
			pullPlanEditorPlan->getTemplate()->shape))->setCheckState(Qt::Checked);

	int thickness = (int) (pullPlanEditorPlan->getTemplate()->getCasingThickness() * 100);
	pullTemplateCasingThicknessSlider->setSliderPosition(thickness);

	int twist = pullPlanEditorPlan->twist;
	pullPlanTwistSlider->setSliderPosition(twist);
	pullPlanTwistSpin->setValue(twist);
	pullPlanEditorViewWidget->repaint();
}

void MainWindow :: pieceTemplateComboBoxChanged(int newIndex)
{
	if (newIndex+1 != pieceEditorPlan->getTemplate()->type)
	{
		pieceEditorPlan->setTemplate(new PieceTemplate(newIndex+1));
		pieceTemplateParameter1Label->setText(pieceEditorPlan->getTemplate()->parameterNames[0]);
		pieceTemplateParameter2Label->setText(pieceEditorPlan->getTemplate()->parameterNames[1]);
		emit someDataChanged();
	}
}



