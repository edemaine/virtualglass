
#include "controller.h"

Controller::Controller(int argc, char **argv)
{
	app = new QApplication(argc, argv);
	app->setStartDragDistance(15);

	model = new Model();
	mainWindow = new MainWindow(model);
}

int Controller::startUp()
{
	mainWindow->show(); 
	mainWindow->seedTable();
	return app->exec();
}





