

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

void MainWindow :: mousePressEvent(QMouseEvent* event)
{
	PullPlanLibraryWidget* pplw = dynamic_cast<PullPlanLibraryWidget*>(childAt(event->pos()));
	if (pplw == NULL)
		return;

	QPixmap pixmap = *pplw->pixmap();

	QMimeData *mimeData = new QMimeData;
	mimeData->setText("0");

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);
	drag->setPixmap(pixmap);
	drag->setHotSpot(QPoint(50, 50)); // - child->pos());

	pplw->setPixmap(pixmap);

	if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction)
		pplw->close();
	else 
	{
		pplw->show();
		pplw->setPixmap(pixmap);
     	}
}

void MainWindow :: dragMoveEvent(QDragMoveEvent* event)
{
	event->acceptProposedAction();
}

void MainWindow :: setupConnections()
{
	connect(pullTemplateComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pullTemplateComboBoxChanged(int)));	
	connect(savePullPlanButton, SIGNAL(pressed()), this, SLOT(savePullPlan()));	
}

void MainWindow :: setupTable()
{
	QVBoxLayout* tableLayout = new QVBoxLayout(centralWidget);
	centralLayout->addLayout(tableLayout, 1);
	QLabel* tableLabel = new QLabel("Table Area");
	tableLayout->addWidget(tableLabel);	

	QWidget* pullPlanLibraryWidget = new QWidget(centralWidget);
	tableLayout->addWidget(pullPlanLibraryWidget);	
	
	pullPlanLibraryLayout = new QHBoxLayout(pullPlanLibraryWidget);
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
	pullPlanEditorPlan = new PullPlan();
	pullPlanEditorPlan->isBase = false;

	defaultPullPlanEditorPlan = new PullPlan();
	defaultPullPlanEditorPlan->setColor(1.0, 1.0, 1.0, 0.1);	
	defaultPullPlanEditorPlan->isBase = true;

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
	pullTemplateGraphicsView->setAcceptDrops(true);
	pullTemplateGraphicsView->setFixedWidth(410);
	pullTemplateGraphicsView->setFixedHeight(410);
	pullTemplateGraphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pullTemplateGraphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pullTemplateGraphicsView->ensureVisible(0, 0, 400, 400, 5, 5);
	editorLayout->addWidget(pullTemplateGraphicsView); 	

	savePullPlanButton = new QPushButton("Save Pull Plan");
	editorLayout->addWidget(savePullPlanButton);

	pullTemplateComboBox->setCurrentIndex(0);
}

void MainWindow :: savePullPlan()
{
	PullPlanLibraryWidget* pplw = new PullPlanLibraryWidget(niceViewWidget);
	pullPlanLibraryLayout->addWidget(pplw);	
}

void MainWindow :: updatePullPlanEditor()
{
	QPen pen;
	pen.setWidth(5);
	pullTemplateGraphicsScene->clear();
	pullTemplateGraphicsScene->addEllipse(0, 0, 400, 400, pen);
	pen.setWidth(10);
	Point upperLeft; 
	PullTemplate* pt = pullPlanEditorPlan->getTemplate();
	for (unsigned int i = 0; i < pt->locations.size(); ++i)
	{
		pen.setColor(QColor(i*20, i*20, i*20));
		upperLeft.x = -pt->diameters[i]/2 * 200.0 + pt->locations[i].x * 200.0;
		upperLeft.y = -pt->diameters[i]/2 * 200.0 + pt->locations[i].y * 200.0;
		pullTemplateGraphicsScene->addEllipse(200.0 + upperLeft.x, 200.0 + upperLeft.y, 
			pt->diameters[i] * 200.0, pt->diameters[i] * 200.0, pen);
	}		
} 

void MainWindow :: updateNiceView()
{
	niceViewWidget->setGeometry(model->getGeometry(pullPlanEditorPlan));	
}

void MainWindow :: pullTemplateComboBoxChanged(int newIndex)
{
	pullPlanEditorPlan->setTemplate(model->getPullTemplate(newIndex+1), defaultPullPlanEditorPlan);
	updatePullPlanEditor();
	updateNiceView();
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

