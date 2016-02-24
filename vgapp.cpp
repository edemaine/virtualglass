
#include <QFileOpenEvent>
#include <QEvent>
#include <QString>
#include "vgapp.h"
#include "randomglass.h"

// the point of subclassing QApplication is just to catch and handle
// request to open files on Mac OS X. Unlike (supposedly) Windows 
// and Linux that give double-clicked files as command line arguments,
// Mac sends an event instead. May be Qt-specific, can't tell.
//
// http://qt-project.org/doc/qt-4.8/qfileopenevent.html
VGApp :: VGApp(int& argc, char **argv ) : QApplication(argc, argv)
{
	randomInit();
	mainWindow = new MainWindow();
	
	// hopefully this catches command line opens on some platforms (Win? Linux?)
	firstOpenRequest = true;
	bool fullscreen = false;
	for (int i = 1; i < argc; ++i)
	{
		if (QString(argv[i]) == QString("-fullscreen"))
			fullscreen = true;
		else if (QString(argv[i]) == QString("-randomcomplexpiece"))
			mainWindow->randomComplexPieceExampleActionTriggered();
		else if (QString(argv[i]) == QString("-randomcomplexcane"))
			mainWindow->randomComplexCaneExampleActionTriggered();
		else
		{
			mainWindow->openFile(QString(argv[i]), !firstOpenRequest);
			firstOpenRequest = false;
		}
	}

	if (fullscreen)
		mainWindow->showFullScreen();
	else	
		mainWindow->showMaximized();
}

VGApp::~VGApp()
{
	// should probably free mainWindow...
}

bool VGApp::event(QEvent *event)
{
	switch (event->type()) 
	{
		case QEvent::FileOpen:
			// and this catches command line opens on the rest (Mac?)
			mainWindow->openFile(static_cast<QFileOpenEvent*>(event)->file(), !firstOpenRequest); 
			firstOpenRequest = false;
			return true;
		default:
			return QApplication::event(event);
	}
}

