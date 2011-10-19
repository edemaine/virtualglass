

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
}

void MainWindow :: mousePressEvent(QMouseEvent* event)
{
	PullPlan* plan;
	QPixmap pixmap;	
	
	PullPlanLibraryWidget* pplw = dynamic_cast<PullPlanLibraryWidget*>(childAt(event->pos()));
	if (pplw == NULL)
	{
		ColorBarLibraryWidget* cblw = dynamic_cast<ColorBarLibraryWidget*>(childAt(event->pos()));
		if (cblw == NULL)
			return;
		else
		{
			plan = cblw->getPullPlan();
			pixmap = *cblw->pixmap();
		}
	}
	else
	{
		plan = pplw->getPullPlan();
		pixmap = *pplw->getEditorPixmap();
	}

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
}

void MainWindow :: setupEditors()
{
	defaultColor.r = defaultColor.g = defaultColor.b = 1.0;
	defaultColor.a = 0.4;

	editorTabs = new QTabWidget(centralWidget);
	centralLayout->addWidget(editorTabs);

	setupPullPlanEditor();
	editorTabs->addTab(pullPlanEditorPage, "Pull Plan");

	setupPickupPlanEditor();
	editorTabs->addTab(pickupPlanEditorPage, "Pickup Plan");
}

void MainWindow :: setupPickupPlanEditor()
{
	pickupPlanEditorPage = new QWidget(editorTabs);
}

void MainWindow :: setupPullPlanEditor()
{
	pullPlanEditorPlan = new PullPlan(LINE_THREE_CIRCLES_TEMPLATE, false, defaultColor);
	pullPlanEditorPlanLibraryWidget = new PullPlanLibraryWidget(QPixmap::fromImage(QImage("./duck.jpg")), QPixmap::fromImage(QImage("./duck.jpg")), pullPlanEditorPlan);
	pullPlanLibraryLayout->addWidget(pullPlanEditorPlanLibraryWidget);	

	pullPlanEditorPage = new QWidget(editorTabs);

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
	updateNiceView();
	updateLibrary();
}

void MainWindow :: updateLibrary()
{
	pullPlanEditorPlanLibraryWidget->updatePixmaps(QPixmap::fromImage(niceViewWidget->renderImage()).scaled(100, 100),
		QPixmap::grabWidget(pullPlanEditorViewWidget).scaled(100, 100));
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
	niceViewWidget->setGeometry(model->getGeometry(pullPlanEditorPlan));	
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

