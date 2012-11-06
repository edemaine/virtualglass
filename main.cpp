
#include <QtGui>
#include "mainwindow.h"

int main(int argc, char** argv)
{
	MainWindow* mainWindow;
	QApplication* app;

	// make X Windows stuff thread-safe?
	QCoreApplication::setAttribute(Qt::AA_X11InitThreads);

	app = new QApplication(argc, argv);

	// drag distance used in drag-n-drop of canes, colors
	app->setStartDragDistance(3);

	// make a decent seed
	qsrand(QDateTime::currentDateTime().toTime_t());

	mainWindow = new MainWindow();
	mainWindow->showMaximized();

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


