
#include "controller.h"

Controller::Controller(int argc, char **argv)
{
	app = new QApplication(argc, argv);
	app->setStartDragDistance(5);
	qsrand(QDateTime::currentDateTime().toTime_t());

	model = new Model();
	mainWindow = new MainWindow(model);
}

int Controller::startUp()
{
	QDir pluginsDir = QDir(app->applicationDirPath());
	#if defined(Q_OS_WIN)
	if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
		pluginsDir.cdUp();
	#elif defined(Q_OS_MAC)
	if (pluginsDir.dirName() == "MacOS") 
	{
		pluginsDir.cdUp();
		pluginsDir.cdUp();
		pluginsDir.cdUp();
	}
	#endif
	mainWindow->show(); 
	mainWindow->seedEverything();
	return app->exec();
}





