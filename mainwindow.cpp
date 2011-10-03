

#include "mainwindow.h"

MainWindow :: MainWindow()
{
	centralWidget = new QWidget(this);
        this->setCentralWidget(centralWidget);

        setWindowTitle(tr("Virtual Glass"));
        resize(1000, 750);
        move(75,25);
}

