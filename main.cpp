
#include <QCoreApplication>
#include <QDir>
#include <Qt>
#include <QDateTime>
#include "vgapp.h"

int main(int argc, char** argv)
{
	// Must come before QApp creation
	// fix Mac OS X 10.9+ font issues
	// https://bugreports.qt-project.org/browse/QTBUG-32789
	// https://bugreports.qt.io/browse/QTBUG-47206	
	#ifdef Q_OS_MACX
	if ( QSysInfo::MacintoshVersion > QSysInfo::MV_10_8 )
	{
		QFont::insertSubstitution(".Lucida Grande UI", "Lucida Grande"); // 10.9
		QFont::insertSubstitution(".Helvetica Neue DeskInterface", "Helvetica Neue"); // 10.9
		QFont::insertSubstitution(".SF NS Text", "Helvetica Neue"); // 10.11
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
	#ifdef Q_OS_WIN
	if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
		pluginsDir.cdUp();
	#endif
	#ifdef Q_OS_MACX
	if (pluginsDir.dirName() == "MacOS")
	{
		pluginsDir.cdUp();
		pluginsDir.cdUp();
		pluginsDir.cdUp();
	}
	#endif
	return app->exec();
}


