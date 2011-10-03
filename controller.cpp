
#include "controller.h"

Controller::Controller(int argc, char **argv)
{
	app = new QApplication(argc, argv);
	model = new Model();
	mainWindow = new MainWindow();
}

int Controller::startUp()
{
	mainWindow->showMaximized();
	return app->exec();
}





