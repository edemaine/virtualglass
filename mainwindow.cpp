

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

void MainWindow :: seedTable()
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

	editorStack->setCurrentIndex(1);
	emit someDataChanged();		
	editorStack->setCurrentIndex(2);
	emit someDataChanged();		
	editorStack->setCurrentIndex(0); // return to 0
	emit someDataChanged();		
}

void MainWindow :: mouseDoubleClickEvent(QMouseEvent* event)
{
	if (!(event->buttons() & Qt::LeftButton))
		return;

	PullPlanLibraryWidget* plplw = dynamic_cast<PullPlanLibraryWidget*>(childAt(event->pos()));
	PickupPlanLibraryWidget* pkplw = dynamic_cast<PickupPlanLibraryWidget*>(childAt(event->pos()));
	PieceLibraryWidget* plw = dynamic_cast<PieceLibraryWidget*>(childAt(event->pos()));

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
	connect(pullTemplateComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pullTemplateComboBoxChanged(int)));	
	connect(newPullPlanButton, SIGNAL(pressed()), this, SLOT(newPullPlan()));	
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(pullPlanEditorViewWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(pullPlanTwistSlider, SIGNAL(valueChanged(int)), this, SLOT(pullPlanTwistSliderChanged(int)));
	connect(pullPlanTwistSpin, SIGNAL(valueChanged(int)), this, SLOT(pullPlanTwistSpinChanged(int)));

	connect(pickupTemplateComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pickupTemplateComboBoxChanged(int)));	
	connect(pickupPlanEditorViewWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(newPickupPlanButton, SIGNAL(pressed()), this, SLOT(newPickupPlan()));	

	connect(pieceTemplateComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pieceTemplateComboBoxChanged(int)));	
	connect(pieceEditorViewWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
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

        colorBarLibraryScrollArea = new QScrollArea(centralWidget);
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

        pullPlanLibraryScrollArea = new QScrollArea;
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

        pickupPlanLibraryScrollArea = new QScrollArea;
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

        pieceLibraryScrollArea = new QScrollArea;
        pieceLibraryScrollArea->setBackgroundRole(QPalette::Dark);
        pieceLibraryScrollArea->setWidget(pieceLibraryWidget);
        pieceLibraryScrollArea->setWidgetResizable(true);
        pieceLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        pieceLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        pieceLibraryScrollArea->setFixedHeight(130);
        pieceLibraryScrollArea->setFixedWidth(500);
	tableLayout->addWidget(pieceLibraryScrollArea);	
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
	pieceEditorPlan = new Piece(SPHERE_TEMPLATE);
	pieceEditorPlanLibraryWidget = new PieceLibraryWidget(QPixmap::fromImage(QImage("./duck.jpg")), 
		QPixmap::fromImage(QImage("./duck.jpg")), pieceEditorPlan);
        pieceLibraryLayout->addWidget(pieceEditorPlanLibraryWidget);

	pieceEditorPage = new QWidget(editorStack);

	QVBoxLayout* editorLayout = new QVBoxLayout(pieceEditorPage);
	pieceEditorPage->setLayout(editorLayout);

	pieceTemplateComboBox = new QComboBox(pieceEditorPage);
	pieceTemplateComboBox->addItem("Sphere");
	pieceTemplateComboBox->addItem("Rollup");
	editorLayout->addWidget(pieceTemplateComboBox, 0);	

	pieceEditorViewWidget = new PieceEditorViewWidget(pieceEditorPlan, pieceEditorPage);
	editorLayout->addWidget(pieceEditorViewWidget, 10); 	
}

void MainWindow :: setupPickupPlanEditor()
{
	pickupPlanEditorPlan = new PickupPlan(THREE_HORIZONTALS_TEMPLATE);
	pickupPlanEditorPlanLibraryWidget = new PickupPlanLibraryWidget(QPixmap::fromImage(QImage("./duck.jpg")), 
		QPixmap::fromImage(QImage("./duck.jpg")), pickupPlanEditorPlan);
        pickupPlanLibraryLayout->addWidget(pickupPlanEditorPlanLibraryWidget);

	pickupPlanEditorPage = new QWidget(editorStack);

	QVBoxLayout* editorLayout = new QVBoxLayout(pickupPlanEditorPage);
	pickupPlanEditorPage->setLayout(editorLayout);

	pickupTemplateComboBox = new QComboBox(pickupPlanEditorPage);
	pickupTemplateComboBox->addItem("Horizontal stack");
	pickupTemplateComboBox->addItem("Vertical stack");
	editorLayout->addWidget(pickupTemplateComboBox, 0);	

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

	pullTemplateComboBox = new QComboBox(pullPlanEditorPage);
	pullTemplateComboBox->addItem("Three circles on a line");
	pullTemplateComboBox->addItem("Five circles on a line");
	pullTemplateComboBox->addItem("Four circles in a square");
	pullTemplateComboBox->addItem("Nine circles in a X");
	pullTemplateComboBox->addItem("Four squares in a square");
	editorLayout->addWidget(pullTemplateComboBox, 0);

	pullPlanEditorViewWidget = new PullPlanEditorViewWidget(pullPlanEditorPlan, pullPlanEditorPage);
	editorLayout->addWidget(pullPlanEditorViewWidget, 10); 	

	QHBoxLayout* twistLayout = new QHBoxLayout(pullPlanEditorPage);
	editorLayout->addLayout(twistLayout);

	pullPlanTwistSpin = new QSpinBox(pullPlanEditorPage);
	pullPlanTwistSpin->setRange(-50, 50);
	pullPlanTwistSpin->setSingleStep(1);
	twistLayout->addWidget(pullPlanTwistSpin, 1);

	pullPlanTwistSlider = new QSlider(Qt::Horizontal, pullPlanEditorPage);
	pullPlanTwistSlider->setRange(-50, 50);
	pullPlanTwistSlider->setTickInterval(5);
	pullPlanTwistSlider->setTickPosition(QSlider::TicksBothSides);
	pullPlanTwistSlider->setSliderPosition(0);
	twistLayout->addWidget(pullPlanTwistSlider, 10);	

	pullTemplateComboBox->setCurrentIndex(0);
}

void MainWindow :: pullPlanTwistSpinChanged(int)
{
        int tick = pullPlanTwistSpin->value();
	pullPlanTwistSlider->setSliderPosition(tick);
	someDataChanged();
}

void MainWindow :: pullPlanTwistSliderChanged(int)
{
        float twist = pullPlanTwistSlider->sliderPosition();
	pullPlanEditorPlan->twist = twist;
	someDataChanged();
}

void MainWindow :: newPickupPlan()
{
	// Create the new plan
	pickupPlanEditorPlan = new PickupPlan(THREE_HORIZONTALS_TEMPLATE);

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
	pickupTemplateComboBox->setCurrentIndex(pickupPlanEditorPlan->getTemplate()->type-1);
}

void MainWindow :: updatePullPlanEditor()
{
	pullPlanEditorViewWidget->repaint();
	int twist = pullPlanEditorPlan->twist;
	pullPlanTwistSlider->setSliderPosition(twist);
	pullPlanTwistSpin->setValue(twist);
	pullTemplateComboBox->setCurrentIndex(pullPlanEditorPlan->getTemplate()->type-1);
} 

void MainWindow :: updateNiceView()
{
	switch (editorStack->currentIndex())
	{
		case 0:
			niceViewWidget->setGeometry(model->getGeometry(pullPlanEditorPlan));	
			break;
		case 1:
			niceViewWidget->setGeometry(model->getGeometry(pickupPlanEditorPlan));	
			break;
		case 2:
			niceViewWidget->setGeometry(model->getGeometry(pieceEditorPlan));	
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

void MainWindow :: pickupTemplateComboBoxChanged(int newIndex)
{
	if (newIndex+1 != pickupPlanEditorPlan->getTemplate()->type)
	{
		pickupPlanEditorPlan->setTemplate(new PickupTemplate(newIndex+1));
		emit someDataChanged();
	}
}

void MainWindow :: pullTemplateComboBoxChanged(int newIndex)
{
	// Only do anything if the change caused the combo box to not match
	// the type of the current template, otherwise we might
	// cause a reset to the default template subcanes instead of 
	// some user specified ones 
	if (newIndex+1 != pullPlanEditorPlan->getTemplate()->type)
	{
		pullPlanEditorPlan->setTemplate(new PullTemplate(newIndex+1));
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

