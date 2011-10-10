

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
        resize(1000, 750);
        move(75,25);
}

void MainWindow :: setupConnections()
{
	connect(pullTemplateComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pullTemplateChanged(int)));	
}

void MainWindow :: setupTable()
{
	QVBoxLayout* tableLayout = new QVBoxLayout(centralWidget);
	centralLayout->addLayout(tableLayout, 1);
	QLabel* tableLabel = new QLabel("Table Area");
	tableLayout->addWidget(tableLabel);	

	QWidget* pullPlanLibraryWidget = new QWidget(centralWidget);
	tableLayout->addWidget(pullPlanLibraryWidget);	
	
	QHBoxLayout* pullPlanLibraryLayout = new QHBoxLayout(pullPlanLibraryWidget);
	pullPlanLibraryLayout->setSpacing(10);
	tableLayout->addLayout(pullPlanLibraryLayout);

        pullPlanLibraryScrollArea = new QScrollArea;
        pullPlanLibraryScrollArea->setBackgroundRole(QPalette::Dark);
        pullPlanLibraryScrollArea->setWidget(pullPlanLibraryWidget);
        pullPlanLibraryScrollArea->setWidgetResizable(true);
        pullPlanLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        pullPlanLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        pullPlanLibraryScrollArea->setFixedHeight(130);

	tableLayout->addWidget(pullPlanLibraryScrollArea);	
}

void MainWindow :: setupPullPlanEditor()
{
	QVBoxLayout* editorLayout = new QVBoxLayout(centralWidget);
	centralLayout->addLayout(editorLayout, 1);

	QLabel* editorLabel = new QLabel("Editor Area");
	editorLayout->addWidget(editorLabel);

	QLabel* templateLabel = new QLabel("Pull Template");
	editorLayout->addWidget(templateLabel);

	pullTemplateComboBox = new QComboBox(centralWidget);
	pullTemplateComboBox->addItem("Three line");
	pullTemplateComboBox->addItem("Five line");
	editorLayout->addWidget(pullTemplateComboBox);

	pullTemplateGraphicsScene = new QGraphicsScene(centralWidget);	
	pullTemplateGraphicsScene->setBackgroundBrush(Qt::gray);

	QGraphicsView* pullTemplateGraphicsView = new QGraphicsView(pullTemplateGraphicsScene, centralWidget);
	pullTemplateGraphicsView->setFixedWidth(410);
	pullTemplateGraphicsView->setFixedHeight(410);
	pullTemplateGraphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pullTemplateGraphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pullTemplateGraphicsView->ensureVisible(0, 0, 400, 400, 5, 5);
	editorLayout->addWidget(pullTemplateGraphicsView); 	
}

void MainWindow :: loadPullTemplate(PullTemplate* pt)
{
	pullTemplateGraphicsScene->clear();
	pullTemplateGraphicsScene->addEllipse(0, 0, 400, 400);
	Point upperLeft; 
	for (unsigned int i = 0; i < pt->locations.size(); ++i)
	{
		upperLeft.x = -pt->diameters[i]/2 * 200.0 + pt->locations[i].x * 200.0;
		upperLeft.y = -pt->diameters[i]/2 * 200.0 + pt->locations[i].y * 200.0;
		pullTemplateGraphicsScene->addEllipse(200.0 + upperLeft.x, 200.0 + upperLeft.y, 
			pt->diameters[i] * 200.0, pt->diameters[i] * 200.0);
	}		
} 

void MainWindow :: pullTemplateChanged(int newIndex)
{
	switch (newIndex+1)
	{
		case LINE_THREE:
			loadPullTemplate(model->getPullTemplate(LINE_THREE));
			break;		
		case LINE_FIVE:
			loadPullTemplate(model->getPullTemplate(LINE_FIVE));
			break;		
	}
}

void MainWindow :: setupNiceView()
{
	niceViewLayout = new QVBoxLayout(centralWidget);
	centralLayout->addLayout(niceViewLayout, 1);

	niceViewLabel = new QLabel("Nice View Area");
	niceViewLayout->addWidget(niceViewLabel);

	niceViewWidget = new NiceViewWidget(centralWidget);
	niceViewLayout->addWidget(niceViewWidget);
}

