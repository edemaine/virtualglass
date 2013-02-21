
#include <QCoreApplication>
#include <QDir>
#include <Qt>
#include <QDateTime>
#include "vgapp.h"

int main(int argc, char** argv)
{
	VGApp* app;

	// make X Windows stuff thread-safe?
	QCoreApplication::setAttribute(Qt::AA_X11InitThreads);
	app = new VGApp(argc, argv);

	// drag distance used in drag-n-drop of canes, colors
	app->setStartDragDistance(3);
	// make a decent seed
	qsrand(QDateTime::currentDateTime().toTime_t());

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


