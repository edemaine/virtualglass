

#include "mainwindow.h"

MainWindow :: MainWindow(Model* model)
{
	centralWidget = new QWidget(this);
        this->setCentralWidget(centralWidget);
	this->model = model;

        centralLayout = new QHBoxLayout(centralWidget);
	setupTable();
	setupEditors();
	setupNiceView();
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

	editorStack->setCurrentIndex(PULLPLAN_EDITOR);  
	emit someDataChanged();		

	// Load pull template types
	for (int i = LINE_THREE_CIRCLES_TEMPLATE; i <= ONE_COLORED_CASING_TEMPLATE; ++i)
	{
		pullPlanEditorPlan->setTemplate(new PullTemplate(i, 0.0));
		pullPlanEditorPlan->getTemplate()->shape = AMORPHOUS_SHAPE; // stop the border from being drawn
		emit someDataChanged();
		PullTemplateLibraryWidget *ptlw = new PullTemplateLibraryWidget(
			QPixmap::grabWidget(pullPlanEditorViewWidget).scaled(100, 100), i);
		pullTemplateLibraryLayout->addWidget(ptlw);
	}

	// Load pull template types
	editorStack->setCurrentIndex(PICKUPPLAN_EDITOR);
	emit someDataChanged();		
	
	for (int i = TWENTY_HORIZONTALS_TEMPLATE; i <= FOUR_SQUARES_OF_TEN_VERTICALS_TEMPLATE; ++i)
	{
		pickupPlanEditorPlan->setTemplate(new PickupTemplate(i));
		emit someDataChanged();
		PickupTemplateLibraryWidget *ptlw = new PickupTemplateLibraryWidget(
			QPixmap::grabWidget(pickupPlanEditorViewWidget).scaled(100, 100), i);
		pickupTemplateLibraryLayout->addWidget(ptlw);
	}

	editorStack->setCurrentIndex(PIECE_EDITOR);
	emit someDataChanged();		

	editorStack->setCurrentIndex(PULLPLAN_EDITOR); // end in pull plan mode
	pullPlanEditorPlan->setTemplate(new PullTemplate(LINE_THREE_CIRCLES_TEMPLATE, 0.0));
	emit someDataChanged();		
}

void MainWindow :: mouseDoubleClickEvent(QMouseEvent* event)
{
	if (!(event->buttons() & Qt::LeftButton))
		return;

	PullPlanLibraryWidget* plplw = dynamic_cast<PullPlanLibraryWidget*>(childAt(event->pos()));
	PickupPlanLibraryWidget* pkplw = dynamic_cast<PickupPlanLibraryWidget*>(childAt(event->pos()));
	PieceLibraryWidget* plw = dynamic_cast<PieceLibraryWidget*>(childAt(event->pos()));
	PullTemplateLibraryWidget* ptlw = dynamic_cast<PullTemplateLibraryWidget*>(childAt(event->pos()));
	PickupTemplateLibraryWidget* pktlw = dynamic_cast<PickupTemplateLibraryWidget*>(childAt(event->pos()));

	if (plplw != NULL)
	{
		pullPlanEditorPlanLibraryWidget = plplw;	
		pullPlanEditorPlan = plplw->getPullPlan();
		pullPlanEditorViewWidget->setPullPlan(pullPlanEditorPlan);
		editorStack->setCurrentIndex(PULLPLAN_EDITOR);
		emit someDataChanged();
	}
	else if (pkplw != NULL)
	{
		pickupPlanEditorPlanLibraryWidget = pkplw;	
		pickupPlanEditorPlan = pkplw->getPickupPlan();
		pickupPlanEditorViewWidget->setPickupPlan(pickupPlanEditorPlan);
		editorStack->setCurrentIndex(PICKUPPLAN_EDITOR);
		emit someDataChanged();
	}
	else if (plw != NULL)
	{
		pieceEditorPlanLibraryWidget = plw;	
		pieceEditorPlan = plw->getPiece();
		pieceEditorViewWidget->setPiece(pieceEditorPlan);
		editorStack->setCurrentIndex(PIECE_EDITOR);
		emit someDataChanged();
	}
	else if (ptlw != NULL)
	{
		pullPlanEditorPlan->setTemplate(new PullTemplate(ptlw->getPullTemplateType(), 0.0));
		emit someDataChanged();
	}
	else if (pktlw != NULL)
	{
		pickupPlanEditorPlan->setTemplate(new PickupTemplate(pktlw->getPickupTemplateType()));
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
	connect(newPickupPlanButton, SIGNAL(pressed()), this, SLOT(newPickupPlan()));	

	connect(pieceTemplateComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pieceTemplateComboBoxChanged(int)));	
	connect(pieceEditorViewWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(newPieceButton, SIGNAL(pressed()), this, SLOT(newPiece()));	
	connect(pieceTemplateParameter1Slider, SIGNAL(valueChanged(int)), 
		this, SLOT(pieceTemplateParameterSlider1Changed(int)));
	connect(pieceTemplateParameter2Slider, SIGNAL(valueChanged(int)), 
		this, SLOT(pieceTemplateParameterSlider2Changed(int)));
}

void MainWindow :: setupTable()
{
	QVBoxLayout* tableLayout = new QVBoxLayout(centralWidget);
	centralLayout->addLayout(tableLayout);

	QHBoxLayout* newThingsLayout = new QHBoxLayout(centralWidget);
	tableLayout->addLayout(newThingsLayout, 0);
 
	newColorButton = new QPushButton("New Color");
	newThingsLayout->addWidget(newColorButton);

	newPullPlanButton = new QPushButton("New Pull Plan");
	newThingsLayout->addWidget(newPullPlanButton);

	newPickupPlanButton = new QPushButton("New Pickup Plan");
	newThingsLayout->addWidget(newPickupPlanButton);
        
	newPieceButton = new QPushButton("New Piece");
	newThingsLayout->addWidget(newPieceButton);

	QWidget* tableGridLibraryWidget = new QWidget(centralWidget);
	tableGridLayout = new QGridLayout(tableGridLibraryWidget);
	tableGridLayout->setSpacing(10);
	tableGridLibraryWidget->setLayout(tableGridLayout);
	colorBarCount = pullPlanCount = pickupPlanCount = pieceCount = 0;

        QScrollArea* tableGridLibraryScrollArea = new QScrollArea;
        tableGridLibraryScrollArea->setBackgroundRole(QPalette::Dark);
        tableGridLibraryScrollArea->setWidget(tableGridLibraryWidget);
        tableGridLibraryScrollArea->setWidgetResizable(true);
        tableGridLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tableGridLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        tableGridLibraryScrollArea->setFixedHeight(700);
        tableGridLibraryScrollArea->setFixedWidth(500);
	tableLayout->addWidget(tableGridLibraryScrollArea);	
}

void MainWindow :: setupEditors()
{
	defaultColor.r = defaultColor.g = defaultColor.b = 1.0;
	defaultColor.a = 0.4;

	editorStack = new QStackedWidget(centralWidget);
	centralLayout->addWidget(editorStack);

	setupPullPlanEditor();
	editorStack->addWidget(pullPlanEditorPage);

	setupPickupPlanEditor();
	editorStack->addWidget(pickupPlanEditorPage);

	setupPieceEditor();
	editorStack->addWidget(pieceEditorPage);
}

void MainWindow :: setupPieceEditor()
{
	pieceEditorPlan = new Piece(TUMBLER_TEMPLATE);
	pieceEditorPlanLibraryWidget = new PieceLibraryWidget(QPixmap::fromImage(QImage("./duck.jpg")), 
		QPixmap::fromImage(QImage("./duck.jpg")), pieceEditorPlan);
	tableGridLayout->addWidget(pieceEditorPlanLibraryWidget, pieceCount, 3);	
	++pieceCount;

	pieceEditorPage = new QWidget(editorStack);

	QVBoxLayout* editorLayout = new QVBoxLayout(pieceEditorPage);
	pieceEditorPage->setLayout(editorLayout);

	pieceTemplateComboBox = new QComboBox(pieceEditorPage);
	pieceTemplateComboBox->addItem("Tumbler");
	pieceTemplateComboBox->addItem("Bowl");
	editorLayout->addWidget(pieceTemplateComboBox, 0);	

	pieceTemplateParameter1Label = new QLabel(pieceEditorPlan->getTemplate()->parameterNames[0]);
	pieceTemplateParameter1Slider = new QSlider(Qt::Horizontal, pieceEditorPage);
	pieceTemplateParameter1Slider->setRange(0, 100);
	pieceTemplateParameter1Slider->setTickPosition(QSlider::TicksBothSides);
	pieceTemplateParameter1Slider->setSliderPosition(0);

	QHBoxLayout* parameter1Layout = new QHBoxLayout(pieceEditorPage);
	editorLayout->addLayout(parameter1Layout);
	parameter1Layout->addWidget(pieceTemplateParameter1Label);
	parameter1Layout->addWidget(pieceTemplateParameter1Slider);

	pieceTemplateParameter2Label = new QLabel(pieceEditorPlan->getTemplate()->parameterNames[1]);
	pieceTemplateParameter2Slider = new QSlider(Qt::Horizontal, pieceEditorPage);
	pieceTemplateParameter2Slider->setRange(0, 100);
	pieceTemplateParameter2Slider->setTickPosition(QSlider::TicksBothSides);
	pieceTemplateParameter2Slider->setSliderPosition(0);

	QHBoxLayout* parameter2Layout = new QHBoxLayout(pieceEditorPage);
	editorLayout->addLayout(parameter2Layout);
	parameter2Layout->addWidget(pieceTemplateParameter2Label);
	parameter2Layout->addWidget(pieceTemplateParameter2Slider);

	pieceEditorViewWidget = new PieceEditorViewWidget(pieceEditorPlan, pieceEditorPage);
	editorLayout->addWidget(pieceEditorViewWidget, 10); 	
}

void MainWindow :: setupPickupPlanEditor()
{
	pickupPlanEditorPlan = new PickupPlan(TWENTY_HORIZONTALS_TEMPLATE);
	pickupPlanEditorPlanLibraryWidget = new PickupPlanLibraryWidget(QPixmap::fromImage(QImage("./duck.jpg")), 
		QPixmap::fromImage(QImage("./duck.jpg")), pickupPlanEditorPlan);
	tableGridLayout->addWidget(pickupPlanEditorPlanLibraryWidget, pickupPlanCount, 2);	
	++pickupPlanCount;

	pickupPlanEditorPage = new QWidget(editorStack);

	QVBoxLayout* editorLayout = new QVBoxLayout(pickupPlanEditorPage);
	pickupPlanEditorPage->setLayout(editorLayout);

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
	editorLayout->addWidget(pickupTemplateLibraryScrollArea);	

	pickupPlanEditorViewWidget = new PickupPlanEditorViewWidget(pickupPlanEditorPlan, pickupPlanEditorPage);
	editorLayout->addWidget(pickupPlanEditorViewWidget, 10); 	
}

void MainWindow :: setupPullPlanEditor()
{
	pullPlanEditorPlan = new PullPlan(LINE_THREE_CIRCLES_TEMPLATE, false, defaultColor);
	pullPlanEditorPlanLibraryWidget = new PullPlanLibraryWidget(QPixmap::fromImage(QImage("./duck.jpg")), QPixmap::fromImage(QImage("./duck.jpg")), pullPlanEditorPlan);
	tableGridLayout->addWidget(pullPlanEditorPlanLibraryWidget, pullPlanCount, 1);	
	++pullPlanCount;

	pullPlanEditorPage = new QWidget(editorStack);

	QVBoxLayout* editorLayout = new QVBoxLayout(pullPlanEditorPage);
	pullPlanEditorPage->setLayout(editorLayout);

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


	pullPlanEditorViewWidget = new PullPlanEditorViewWidget(pullPlanEditorPlan, pullPlanEditorPage);
	editorLayout->addWidget(pullPlanEditorViewWidget, 10); 	

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

	pieceEditorPlanLibraryWidget = new PieceLibraryWidget(QPixmap::fromImage(QImage("./duck.jpg")), 
		QPixmap::fromImage(QImage("./duck.jpg")), pieceEditorPlan);
	tableGridLayout->addWidget(pieceEditorPlanLibraryWidget, pieceCount, 3);
	++pieceCount;

	pieceEditorViewWidget->setPiece(pieceEditorPlan);
	
	// Load up the right editor
	editorStack->setCurrentIndex(PIECE_EDITOR);

	emit someDataChanged();
}

void MainWindow :: newPickupPlan()
{
	// Create the new plan
	pickupPlanEditorPlan = new PickupPlan(TWENTY_HORIZONTALS_TEMPLATE);

	// Create the new library entry
	pickupPlanEditorPlanLibraryWidget = new PickupPlanLibraryWidget(QPixmap::fromImage(QImage("./duck.jpg")), 
		QPixmap::fromImage(QImage("./duck.jpg")), pickupPlanEditorPlan);
	tableGridLayout->addWidget(pickupPlanEditorPlanLibraryWidget, pickupPlanCount, 2);
	++pickupPlanCount;

	// Give the new plan to the editor
	pickupPlanEditorViewWidget->setPickupPlan(pickupPlanEditorPlan);

	// Load up the right editor
	editorStack->setCurrentIndex(PICKUPPLAN_EDITOR);

	// Trigger GUI updates
	emit someDataChanged();
}

void MainWindow :: newPullPlan()
{
	// Create the new plan
	pullPlanEditorPlan = new PullPlan(LINE_THREE_CIRCLES_TEMPLATE, false, defaultColor); 

	// Create the new library entry
	pullPlanEditorPlanLibraryWidget = new PullPlanLibraryWidget(QPixmap::fromImage(QImage("./duck.jpg")), 
		QPixmap::fromImage(QImage("./duck.jpg")), pullPlanEditorPlan);
	tableGridLayout->addWidget(pullPlanEditorPlanLibraryWidget, pullPlanCount, 1);
	++pullPlanCount;

	// Give the new plan to the editor
	pullPlanEditorViewWidget->setPullPlan(pullPlanEditorPlan);

	// Load up the right editor
	editorStack->setCurrentIndex(PULLPLAN_EDITOR);

	// Trigger GUI updates
	emit someDataChanged();
}

void MainWindow :: updateEverything()
{
	updatePullPlanEditor();
	updatePickupPlanEditor();
	updatePieceEditor();
	updateNiceView();
	updateLibrary();
}

void MainWindow :: updateLibrary()
{
	switch (editorStack->currentIndex())
	{
		case PULLPLAN_EDITOR:
			pullPlanEditorPlanLibraryWidget->updatePixmaps(
				QPixmap::fromImage(niceViewWidget->renderImage()).scaled(100, 100), 
				QPixmap::grabWidget(pullPlanEditorViewWidget).scaled(100, 100));
			break;
		case PICKUPPLAN_EDITOR:
			pickupPlanEditorPlanLibraryWidget->updatePixmaps(
				QPixmap::grabWidget(pickupPlanEditorViewWidget).scaled(100, 100),
				QPixmap::grabWidget(pickupPlanEditorViewWidget).scaled(100, 100));
			break;
		case PIECE_EDITOR:
			pieceEditorPlanLibraryWidget->updatePixmaps(
				QPixmap::fromImage(niceViewWidget->renderImage()).scaled(100, 100),
				QPixmap::fromImage(niceViewWidget->renderImage()).scaled(100, 100));
			break;
	}
}

void MainWindow :: updatePieceEditor()
{
	pieceEditorViewWidget->repaint();
	pieceTemplateComboBox->setCurrentIndex(pieceEditorPlan->getTemplate()->type-1);
}

void MainWindow :: updatePickupPlanEditor()
{
	pickupPlanEditorViewWidget->repaint();
}

void MainWindow :: updatePullPlanEditor()
{
	// Only attempt to set the shape if it's defined; it's undefined during loading
	if (pullPlanEditorPlan->getTemplate()->shape < AMORPHOUS_SHAPE)
		static_cast<QCheckBox*>(pullTemplateShapeButtonGroup->button(pullPlanEditorPlan->getTemplate()->shape))->setCheckState(Qt::Checked);

        int thickness = (int) (pullPlanEditorPlan->getTemplate()->getCasingThickness() * 100);
        pullTemplateCasingThicknessSlider->setSliderPosition(thickness);

	int twist = pullPlanEditorPlan->twist;
	pullPlanTwistSlider->setSliderPosition(twist);
	pullPlanTwistSpin->setValue(twist);
	pullPlanEditorViewWidget->repaint();
} 

void MainWindow :: updateNiceView()
{
	Geometry* geometry;

	switch (editorStack->currentIndex())
	{
		case PULLPLAN_EDITOR:
 			geometry = model->getGeometry(pullPlanEditorPlan);
			niceViewWidget->setCameraMode(PULLPLAN_MODE);
			break;
		case PICKUPPLAN_EDITOR:
 			geometry = model->getGeometry(pickupPlanEditorPlan);
			niceViewWidget->setCameraMode(PICKUPPLAN_MODE);
			break;
		case PIECE_EDITOR:
 			geometry = model->getGeometry(pieceEditorPlan);
			niceViewWidget->setCameraMode(PIECE_MODE);
			break;
		default:
			exit(1);
	}

	niceViewWidget->setGeometry(geometry);
	if (writeRawCheckBox->checkState() == Qt::Checked)	
		geometry->save_raw_file("./cane.raw");
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

void MainWindow :: setupNiceView()
{
	niceViewLayout = new QVBoxLayout(centralWidget);
	centralLayout->addLayout(niceViewLayout);

	niceViewWidget = new NiceViewWidget(centralWidget);
	niceViewLayout->addWidget(niceViewWidget);

	writeRawCheckBox = new QCheckBox("Write .raw file", centralWidget);
	writeRawCheckBox->setCheckState(Qt::Unchecked);
	niceViewLayout->addWidget(writeRawCheckBox, 0);
}


