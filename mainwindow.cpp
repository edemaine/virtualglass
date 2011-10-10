

#include "mainwindow.h"

MainWindow :: MainWindow()
{
	centralWidget = new QWidget(this);
        this->setCentralWidget(centralWidget);

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
	
}

void MainWindow :: setupTable()
{
	QLabel* tableLabel = new QLabel("Table Area");
	centralLayout->addWidget(tableLabel);
}

void MainWindow :: setupPullPlanEditor()
{
	editorLayout = new QFormLayout(centralWidget);
	centralLayout->addLayout(editorLayout);

	editorLabel = new QLabel("Editor Area");
	editorLayout->addWidget(editorLabel);
	pullTemplateComboBox = new QComboBox(centralWidget);
	pullTemplateComboBox->addItem("Big X");
	pullTemplateComboBox->addItem("Small X");
	pullTemplateComboBox->addItem("Big H");
	pullTemplateComboBox->addItem("Small H");
        editorLayout->addRow("Pull Template:", pullTemplateComboBox);
}

void MainWindow :: setupNiceView()
{
	niceViewLayout = new QVBoxLayout(centralWidget);
	centralLayout->addLayout(niceViewLayout);

	niceViewLabel = new QLabel("Nice View Area");
	niceViewLayout->addWidget(niceViewLabel);

	niceViewWidget = new NiceViewWidget(centralWidget);
	niceViewLayout->addWidget(niceViewWidget);
}

