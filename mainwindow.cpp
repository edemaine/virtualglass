

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
	seedTable();
	emit someDataChanged();

        setWindowTitle(tr("Virtual Glass"));
        resize(1000, 750);
        move(75,25);
}

void MainWindow :: seedTable()
{
			
}

void MainWindow :: mousePressEvent(QMouseEvent* event)
{
	PullPlanLibraryWidget* pplw = dynamic_cast<PullPlanLibraryWidget*>(childAt(event->pos()));
	if (pplw == NULL)
		return;

	QPixmap pixmap = *pplw->pixmap();

	PullPlan* fakePlan = new PullPlan();
	fakePlan->isBase = true;
	fakePlan->setColor(1.0, 0.5, 0.5, 0.4);
	char buf[128];
	sprintf(buf, "%p", fakePlan);//pplw->getPullPlan());
	QByteArray pointerData(buf); 
	QMimeData* mimeData = new QMimeData;
	mimeData->setText(pointerData);

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);
	drag->setPixmap(pixmap);
	drag->setHotSpot(QPoint(50, 50)); 

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
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(pullTemplateGraphicsView, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
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
	pullPlanEditorPlan->setTemplate(model->getPullTemplate(1), defaultPullPlanEditorPlan);

	QVBoxLayout* editorLayout = new QVBoxLayout(centralWidget);
	centralLayout->addLayout(editorLayout, 1);

	QLabel* templateLabel = new QLabel("Pull Template");
	editorLayout->addWidget(templateLabel, 0);

	pullTemplateComboBox = new QComboBox(centralWidget);
	pullTemplateComboBox->addItem("Three line");
	pullTemplateComboBox->addItem("Five line");
	pullTemplateComboBox->addItem("Four square");
	editorLayout->addWidget(pullTemplateComboBox, 0);

	pullTemplateGraphicsView = new PullTemplateGraphicsView(pullPlanEditorPlan, centralWidget);
	editorLayout->addWidget(pullTemplateGraphicsView, 10); 	

	savePullPlanButton = new QPushButton("Save Pull Plan");
	editorLayout->addWidget(savePullPlanButton);

	pullTemplateComboBox->setCurrentIndex(0);
}

void MainWindow :: savePullPlan()
{
	PullPlanLibraryWidget* pplw = new PullPlanLibraryWidget(niceViewWidget, pullPlanEditorPlan);
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
	centralLayout->addLayout(niceViewLayout, 1);

	niceViewWidget = new NiceViewWidget(centralWidget);
	niceViewLayout->addWidget(niceViewWidget);
}

