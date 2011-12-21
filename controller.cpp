
#include "controller.h"

Controller::Controller(int argc, char **argv)
{
	app = new QApplication(argc, argv);
	app->setStartDragDistance(3);
	qsrand(QDateTime::currentDateTime().toTime_t());

	mainWindow = new MainWindow();
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
	return app->exec();
}





