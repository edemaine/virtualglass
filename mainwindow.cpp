

#include "mainwindow.h"

MainWindow :: MainWindow(Model* model)
{
	centralWidget = new QWidget(this);
        this->setCentralWidget(centralWidget);
	this->model = model;

        centralLayout = new QHBoxLayout(centralWidget);
	setupTable();
	setupPullPlanEditor();
	setupNiceView();
	setupConnections();

        setWindowTitle(tr("Virtual Glass"));
        move(0, 0);
}

void MainWindow :: seedTable()
{
	// Sampling of Reichenbach colors from Kim's color file
	ColorBarLibraryWidget* cblw = new ColorBarLibraryWidget(255, 255, 255, 100);
	colorBarLibraryLayout->addWidget(cblw);	
	cblw = new ColorBarLibraryWidget(1, 58, 186, 128);
	colorBarLibraryLayout->addWidget(cblw);	
	cblw = new ColorBarLibraryWidget(222, 205, 1, 126);
	colorBarLibraryLayout->addWidget(cblw);	
	cblw = new ColorBarLibraryWidget(2, 101, 35, 128);
	colorBarLibraryLayout->addWidget(cblw);	
	cblw = new ColorBarLibraryWidget(253, 122, 56, 128);
	colorBarLibraryLayout->addWidget(cblw);	
	cblw = new ColorBarLibraryWidget(226, 190, 161, 255);
	colorBarLibraryLayout->addWidget(cblw);	
	cblw = new ColorBarLibraryWidget(50, 102, 54, 255);
	colorBarLibraryLayout->addWidget(cblw);	
	cblw = new ColorBarLibraryWidget(60, 31, 37, 255);
	colorBarLibraryLayout->addWidget(cblw);	
	cblw = new ColorBarLibraryWidget(131, 149, 201, 255);
	colorBarLibraryLayout->addWidget(cblw);	
	cblw = new ColorBarLibraryWidget(138, 155, 163, 255);
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
		pixmap = pplw->getEditorPixmap();
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
	connect(savePullPlanButton, SIGNAL(pressed()), this, SLOT(savePullPlan()));	
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(pullTemplateGraphicsView, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
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
}

void MainWindow :: setupPullPlanEditor()
{
	defaultPullPlanEditorPlan = new PullPlan();
	defaultPullPlanEditorPlan->setColor(255, 255, 255, 100);
	defaultPullPlanEditorPlan->isBase = true;

	pullPlanEditorPlan = new PullPlan();
	pullPlanEditorPlan->isBase = false;
	pullPlanEditorPlan->setTemplate(model->getPullTemplate(1), defaultPullPlanEditorPlan);

	QVBoxLayout* editorLayout = new QVBoxLayout(centralWidget);
	centralLayout->addLayout(editorLayout);

	pullTemplateComboBox = new QComboBox(centralWidget);
	pullTemplateComboBox->addItem("Three line");
	pullTemplateComboBox->addItem("Five line");
	pullTemplateComboBox->addItem("Four square");
	pullTemplateComboBox->addItem("Nine X");
	editorLayout->addWidget(pullTemplateComboBox, 0);

	pullTemplateGraphicsView = new PullTemplateGraphicsView(pullPlanEditorPlan, centralWidget);
	editorLayout->addWidget(pullTemplateGraphicsView, 10); 	

	QHBoxLayout* twistLayout = new QHBoxLayout(centralWidget);
	editorLayout->addLayout(twistLayout);

	pullPlanTwistSpin = new QSpinBox(centralWidget);
	pullPlanTwistSpin->setRange(-50, 50);
	pullPlanTwistSpin->setSingleStep(1);
	twistLayout->addWidget(pullPlanTwistSpin, 1);

	pullPlanTwistSlider = new QSlider(Qt::Horizontal, centralWidget);
	pullPlanTwistSlider->setRange(-50, 50);
	pullPlanTwistSlider->setTickInterval(5);
	pullPlanTwistSlider->setTickPosition(QSlider::TicksBothSides);
	pullPlanTwistSlider->setSliderPosition(0);
	twistLayout->addWidget(pullPlanTwistSlider, 10);	

	savePullPlanButton = new QPushButton("Save Pull Plan");
	editorLayout->addWidget(savePullPlanButton);

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
	pullPlanEditorPlan->setTwist(twist);
	someDataChanged();
}

void MainWindow :: savePullPlan()
{
	PullPlanLibraryWidget* pplw = new PullPlanLibraryWidget(
		QPixmap::fromImage(niceViewWidget->renderImage()).scaled(100, 100), 
		QPixmap::grabWidget(pullTemplateGraphicsView).scaled(100, 100), pullPlanEditorPlan);
	pullPlanLibraryLayout->addWidget(pplw);	
}

void MainWindow :: updateEverything()
{
	updatePullPlanEditor();
	updateNiceView();
}

void MainWindow :: updatePullPlanEditor()
{
	pullTemplateGraphicsView->repaint();
	int twist = pullPlanEditorPlan->getTwist();
	pullPlanTwistSlider->setSliderPosition(twist);
	pullPlanTwistSpin->setValue(twist);
} 

void MainWindow :: updateNiceView()
{
	niceViewWidget->setGeometry(model->getGeometry(pullPlanEditorPlan));	
}

void MainWindow :: pullTemplateComboBoxChanged(int newIndex)
{
	pullPlanEditorPlan->setTemplate(model->getPullTemplate(newIndex+1), defaultPullPlanEditorPlan);
	emit someDataChanged();
}

void MainWindow :: setupNiceView()
{
	niceViewLayout = new QVBoxLayout(centralWidget);
	centralLayout->addLayout(niceViewLayout);

	niceViewWidget = new NiceViewWidget(centralWidget);
	niceViewLayout->addWidget(niceViewWidget);
}

