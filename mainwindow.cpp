

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
	colorBarLibraryLayout->addWidget(cblw);	

	color.r = 1/255.0;
	color.g = 58/255.0;
	color.b = 186/255.0;
	color.a = 128/255.0;
	cblw = new ColorBarLibraryWidget(color);
	colorBarLibraryLayout->addWidget(cblw);	

	color.r = 222/255.0;
	color.g = 205/255.0;
	color.b = 1/255.0;
	color.a = 126/255.0;
	cblw = new ColorBarLibraryWidget(color);
	colorBarLibraryLayout->addWidget(cblw);	

	color.r = 2/255.0;
	color.g = 101/255.0;
	color.b = 35/255.0;
	color.a = 128/255.0;
	cblw = new ColorBarLibraryWidget(color);
	colorBarLibraryLayout->addWidget(cblw);	

	color.r = 253/255.0;
	color.g = 122/255.0;
	color.b = 56/255.0;
	color.a = 128/255.0;
	cblw = new ColorBarLibraryWidget(color);
	colorBarLibraryLayout->addWidget(cblw);	

	color.r = 226/255.0;
	color.g = 190/255.0;
	color.b = 161/255.0;
	color.a = 255/255.0;
	cblw = new ColorBarLibraryWidget(color);
	colorBarLibraryLayout->addWidget(cblw);	

	color.r = 50/255.0;
	color.g = 102/255.0;
	color.b = 54/255.0;
	color.a = 255/255.0;
	cblw = new ColorBarLibraryWidget(color);
	colorBarLibraryLayout->addWidget(cblw);	

	color.r = 60/255.0;
	color.g = 31/255.0;
	color.b = 37/255.0;
	color.a = 255/255.0;
	cblw = new ColorBarLibraryWidget(color);
	colorBarLibraryLayout->addWidget(cblw);	

	color.r = 131/255.0;
	color.g = 149/255.0;
	color.b = 201/255.0;
	color.a = 255/255.0;
	cblw = new ColorBarLibraryWidget(color);
	colorBarLibraryLayout->addWidget(cblw);	

	color.r = 138/255.0;
	color.g = 155/255.0;
	color.b = 163/255.0;
	color.a = 255/255.0;
	cblw = new ColorBarLibraryWidget(color);
	colorBarLibraryLayout->addWidget(cblw);	

	color.r = 255/255.0;
	color.g = 255/255.0;
	color.b = 255/255.0;
	color.a = 10/255.0;
	cblw = new ColorBarLibraryWidget(color);
	colorBarLibraryLayout->addWidget(cblw);	

	// setup the editor/3D view
	emit someDataChanged();		

	editorStack->setCurrentIndex(0);  
	emit someDataChanged();		

	// Load pull template types
	for (int i = LINE_THREE_CIRCLES_TEMPLATE; i <= SQUARE_FOUR_SQUARES_TEMPLATE; ++i)
	{
		pullPlanEditorPlan->setTemplate(new PullTemplate(i, 0.0));
		pullPlanEditorPlan->getTemplate()->shape = AMORPHOUS_SHAPE; // stop the border from being drawn
		emit someDataChanged();
		PullTemplateLibraryWidget *ptlw = new PullTemplateLibraryWidget(
			QPixmap::grabWidget(pullPlanEditorViewWidget).scaled(100, 100), i);
		pullTemplateLibraryLayout->addWidget(ptlw);
	}

	// Load pull template types
	editorStack->setCurrentIndex(1);
	emit someDataChanged();		
	
	for (int i = TEN_HORIZONTALS_TEMPLATE; i <= SIX_COLUMNS_OF_TEN_HORIZONTALS_TEMPLATE; ++i)
	{
		pickupPlanEditorPlan->setTemplate(new PickupTemplate(i));
		emit someDataChanged();
		PickupTemplateLibraryWidget *ptlw = new PickupTemplateLibraryWidget(
			QPixmap::grabWidget(pickupPlanEditorViewWidget).scaled(100, 100), i);
		pickupTemplateLibraryLayout->addWidget(ptlw);
	}

	editorStack->setCurrentIndex(2);
	emit someDataChanged();		

	editorStack->setCurrentIndex(0); // end in pull plan mode
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
		editorStack->setCurrentIndex(0);
		emit someDataChanged();
	}
	else if (pkplw != NULL)
	{
		pickupPlanEditorPlanLibraryWidget = pkplw;	
		pickupPlanEditorPlan = pkplw->getPickupPlan();
		pickupPlanEditorViewWidget->setPickupPlan(pickupPlanEditorPlan);
		editorStack->setCurrentIndex(1);
		emit someDataChanged();
	}
	else if (plw != NULL)
	{
		pieceEditorPlanLibraryWidget = plw;	
		pieceEditorPlan = plw->getPiece();
		pieceEditorViewWidget->setPiece(pieceEditorPlan);
		editorStack->setCurrentIndex(2);
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
	if (cblw != NULL)
	{
		plan = cblw->getPullPlan();
		pixmap = *cblw->pixmap();
	}
	else if (plplw != NULL)
	{
		plan = plplw->getPullPlan();
		pixmap = *plplw->getEditorPixmap();
	}
	else if (pkplw != NULL)
	{
		plan = pkplw->getPickupPlan();
		pixmap = *pkplw->getEditorPixmap();
	}
	else if (plw != NULL)
	{
		plan = plw->getPiece();
		pixmap = *plw->getEditorPixmap();
	}
	else
		return;

	char buf[128];
	sprintf(buf, "%p", plan);
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
}

void MainWindow :: setupTable()
{
	QVBoxLayout* tableLayout = new QVBoxLayout(centralWidget);
	centralLayout->addLayout(tableLayout);

	// Setup color bar scrolling library
	QWidget* colorBarLibraryWidget = new QWidget(centralWidget);
	colorBarLibraryLayout = new QHBoxLayout(colorBarLibraryWidget);
	colorBarLibraryLayout->setSpacing(10);
	colorBarLibraryWidget->setLayout(colorBarLibraryLayout);

        QScrollArea* colorBarLibraryScrollArea = new QScrollArea(centralWidget);
        colorBarLibraryScrollArea->setBackgroundRole(QPalette::Dark);
        colorBarLibraryScrollArea->setWidget(colorBarLibraryWidget);
        colorBarLibraryScrollArea->setWidgetResizable(true);
        colorBarLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        colorBarLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        colorBarLibraryScrollArea->setFixedHeight(130);
        colorBarLibraryScrollArea->setFixedWidth(500);
	tableLayout->addWidget(colorBarLibraryScrollArea);	

	// Setup pull plan scrolling library
	QWidget* pullPlanLibraryWidget = new QWidget(centralWidget);
	pullPlanLibraryLayout = new QHBoxLayout(pullPlanLibraryWidget);
	pullPlanLibraryLayout->setSpacing(10);
	pullPlanLibraryWidget->setLayout(pullPlanLibraryLayout);

        QScrollArea* pullPlanLibraryScrollArea = new QScrollArea;
        pullPlanLibraryScrollArea->setBackgroundRole(QPalette::Dark);
        pullPlanLibraryScrollArea->setWidget(pullPlanLibraryWidget);
        pullPlanLibraryScrollArea->setWidgetResizable(true);
        pullPlanLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        pullPlanLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        pullPlanLibraryScrollArea->setFixedHeight(130);
        pullPlanLibraryScrollArea->setFixedWidth(500);
	tableLayout->addWidget(pullPlanLibraryScrollArea);	

	newPullPlanButton = new QPushButton("New Pull Plan");
	tableLayout->addWidget(newPullPlanButton);

	// Setup pickup plan scrolling library
	QWidget* pickupPlanLibraryWidget = new QWidget(centralWidget);
	pickupPlanLibraryLayout = new QHBoxLayout(pickupPlanLibraryWidget);
	pickupPlanLibraryLayout->setSpacing(10);
	pickupPlanLibraryWidget->setLayout(pickupPlanLibraryLayout);

        QScrollArea* pickupPlanLibraryScrollArea = new QScrollArea;
        pickupPlanLibraryScrollArea->setBackgroundRole(QPalette::Dark);
        pickupPlanLibraryScrollArea->setWidget(pickupPlanLibraryWidget);
        pickupPlanLibraryScrollArea->setWidgetResizable(true);
        pickupPlanLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        pickupPlanLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        pickupPlanLibraryScrollArea->setFixedHeight(130);
        pickupPlanLibraryScrollArea->setFixedWidth(500);
	tableLayout->addWidget(pickupPlanLibraryScrollArea);	

	newPickupPlanButton = new QPushButton("New Pickup Plan");
	tableLayout->addWidget(newPickupPlanButton);

	// Setup piece scrolling library
	QWidget* pieceLibraryWidget = new QWidget(centralWidget);
	pieceLibraryLayout = new QHBoxLayout(pieceLibraryWidget);
	pieceLibraryLayout->setSpacing(10);
	pieceLibraryWidget->setLayout(pieceLibraryLayout);

        QScrollArea* pieceLibraryScrollArea = new QScrollArea;
        pieceLibraryScrollArea->setBackgroundRole(QPalette::Dark);
        pieceLibraryScrollArea->setWidget(pieceLibraryWidget);
        pieceLibraryScrollArea->setWidgetResizable(true);
        pieceLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        pieceLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        pieceLibraryScrollArea->setFixedHeight(130);
        pieceLibraryScrollArea->setFixedWidth(500);
	tableLayout->addWidget(pieceLibraryScrollArea);	

	newPieceButton = new QPushButton("New Piece");
	tableLayout->addWidget(newPieceButton);
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
	pieceEditorPlan = new Piece(WAVY_ONE_TEMPLATE);
	pieceEditorPlanLibraryWidget = new PieceLibraryWidget(QPixmap::fromImage(QImage("./duck.jpg")), 
		QPixmap::fromImage(QImage("./duck.jpg")), pieceEditorPlan);
        pieceLibraryLayout->addWidget(pieceEditorPlanLibraryWidget);

	pieceEditorPage = new QWidget(editorStack);

	QVBoxLayout* editorLayout = new QVBoxLayout(pieceEditorPage);
	pieceEditorPage->setLayout(editorLayout);

	pieceTemplateComboBox = new QComboBox(pieceEditorPage);
	pieceTemplateComboBox->addItem("Wavy 1");
	pieceTemplateComboBox->addItem("Wavy 2");
	pieceTemplateComboBox->addItem("Wavy 3");
	pieceTemplateComboBox->addItem("Wavy 4");
	pieceTemplateComboBox->addItem("Rollup");
	editorLayout->addWidget(pieceTemplateComboBox, 0);	

	pieceEditorViewWidget = new PieceEditorViewWidget(pieceEditorPlan, pieceEditorPage);
	editorLayout->addWidget(pieceEditorViewWidget, 10); 	
}

void MainWindow :: setupPickupPlanEditor()
{
	pickupPlanEditorPlan = new PickupPlan(TEN_HORIZONTALS_TEMPLATE);
	pickupPlanEditorPlanLibraryWidget = new PickupPlanLibraryWidget(QPixmap::fromImage(QImage("./duck.jpg")), 
		QPixmap::fromImage(QImage("./duck.jpg")), pickupPlanEditorPlan);
        pickupPlanLibraryLayout->addWidget(pickupPlanEditorPlanLibraryWidget);

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
	pullPlanLibraryLayout->addWidget(pullPlanEditorPlanLibraryWidget);	

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
	pieceEditorPlan = new Piece(WAVY_ONE_TEMPLATE);

	pieceEditorPlanLibraryWidget = new PieceLibraryWidget(QPixmap::fromImage(QImage("./duck.jpg")), 
		QPixmap::fromImage(QImage("./duck.jpg")), pieceEditorPlan);
	pieceLibraryLayout->addWidget(pieceEditorPlanLibraryWidget);	

	pieceEditorViewWidget->setPiece(pieceEditorPlan);

	emit someDataChanged();
}

void MainWindow :: newPickupPlan()
{
	// Create the new plan
	pickupPlanEditorPlan = new PickupPlan(TEN_HORIZONTALS_TEMPLATE);

	// Create the new library entry
	pickupPlanEditorPlanLibraryWidget = new PickupPlanLibraryWidget(QPixmap::fromImage(QImage("./duck.jpg")), 
		QPixmap::fromImage(QImage("./duck.jpg")), pickupPlanEditorPlan);
	pickupPlanLibraryLayout->addWidget(pickupPlanEditorPlanLibraryWidget);	

	// Give the new plan to the editor
	pickupPlanEditorViewWidget->setPickupPlan(pickupPlanEditorPlan);

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
	pullPlanLibraryLayout->addWidget(pullPlanEditorPlanLibraryWidget);	

	// Give the new plan to the editor
	pullPlanEditorViewWidget->setPullPlan(pullPlanEditorPlan);

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
		case 0:
			pullPlanEditorPlanLibraryWidget->updatePixmaps(
				QPixmap::fromImage(niceViewWidget->renderImage()).scaled(100, 100), 
				QPixmap::grabWidget(pullPlanEditorViewWidget).scaled(100, 100));
			break;
		case 1:
			pickupPlanEditorPlanLibraryWidget->updatePixmaps(
				QPixmap::grabWidget(pickupPlanEditorViewWidget).scaled(100, 100),
				QPixmap::grabWidget(pickupPlanEditorViewWidget).scaled(100, 100));
			break;
		case 2:
			pieceEditorPlanLibraryWidget->updatePixmaps(
				QPixmap::grabWidget(pieceEditorViewWidget).scaled(100, 100),
				QPixmap::grabWidget(pieceEditorViewWidget).scaled(100, 100));
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
	switch (editorStack->currentIndex())
	{
		case 0:
			niceViewWidget->setGeometry(model->getGeometry(pullPlanEditorPlan));	
			niceViewWidget->setCameraMode(PULLPLAN_MODE);
			break;
		case 1:
			niceViewWidget->setGeometry(model->getGeometry(pickupPlanEditorPlan));	
			niceViewWidget->setCameraMode(PICKUPPLAN_MODE);
			break;
		case 2:
			niceViewWidget->setGeometry(model->getGeometry(pieceEditorPlan));	
			niceViewWidget->setCameraMode(PIECE_MODE);
			break;
	}
}

void MainWindow :: pieceTemplateComboBoxChanged(int newIndex)
{
	if (newIndex+1 != pieceEditorPlan->getTemplate()->type)
	{
		pieceEditorPlan->setTemplate(new PieceTemplate(newIndex+1));
		emit someDataChanged();
	}
}

void MainWindow :: setupNiceView()
{
	niceViewLayout = new QVBoxLayout(centralWidget);
	centralLayout->addLayout(niceViewLayout);

	niceViewWidget = new NiceViewWidget(centralWidget);
	niceViewLayout->addWidget(niceViewWidget);
}

