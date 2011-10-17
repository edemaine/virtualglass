
#include "controller.h"

Controller::Controller(int argc, char **argv)
{
	app = new QApplication(argc, argv);
	model = new Model();
	mainWindow = new MainWindow(model);
}

int Controller::startUp()
{
	mainWindow->showMaximized();
	mainWindow->seedTable();
	return app->exec();
}





