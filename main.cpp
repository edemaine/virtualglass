
#include <QCoreApplication>
#include <QDir>
#include <Qt>
#include <QDateTime>
#include "vgapp.h"

int main(int argc, char** argv)
{
	// Must come before QApp creation
	#if defined(Q_OS_MACX)
	// Font fix from http://successfulsoftware.net/2013/10/23/fixing-qt-4-for-mac-os-x-10-9-mavericks/
	if ( QSysInfo::MacintoshVersion > QSysInfo::MV_10_8 )
	{
		// fix Mac OS X 10.9 (mavericks) font issue
		// https://bugreports.qt-project.org/browse/QTBUG-32789
		QFont::insertSubstitution(".Lucida Grande UI", "Lucida Grande");
	}
	#endif

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
	#elif defined(Q_OS_MACX)
	if (pluginsDir.dirName() == "MacOS")
	{
		pluginsDir.cdUp();
		pluginsDir.cdUp();
		pluginsDir.cdUp();
	}
	#endif
	return app->exec();
}


