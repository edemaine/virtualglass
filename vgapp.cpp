
#include <QFileOpenEvent>
#include <QEvent>
#include <QString>
#include <QMessageBox>

#include "vgapp.h"
#include "randomglass.h"
#include "email.h"
#include "globalmuseumsetting.h"
#include "globaldepthpeelingsetting.h"

VGApp :: VGApp(int& argc, char **argv ) : QApplication(argc, argv)
{
	randomInit();
	
	// Preprocess command-line arguments to look for -museum and -[no]gpu.
	// Need to set this *before* the GUI is launched, for MainWindow init.
	for (int i = 1; i < argc; ++i)
	{
		if (QString(argv[i]) == QString("-museum"))
		{
			GlobalMuseumSetting::setEnabled(true);
			break;
		}
		else if (QString(argv[i]) == QString("-gpu"))
			GlobalDepthPeelingSetting::setEnabled(true);
		else if (QString(argv[i]) == QString("-nogpu"))
			GlobalDepthPeelingSetting::setEnabled(false);
	}

	mainWindow = new MainWindow();
	
	// All other command line arguments (use --args to pass these in on OS X)
	firstOpenRequest = true;
	bool fullscreen = false;
	for (int i = 1; i < argc; ++i)
	{
		//QMessageBox::warning(mainWindow, "Argument", "Argument " + QString::number(i) + " is " + argv[i]);
		if (QString(argv[i]) == QString("-fullscreen"))
			fullscreen = true;
		else if (QString(argv[i]) == QString("-randomcomplexpiece"))
			mainWindow->randomComplexPieceExampleActionTriggered();
		else if (QString(argv[i]) == QString("-randomcomplexcane"))
			mainWindow->randomComplexCaneExampleActionTriggered();
		else if (QString(argv[i]) == QString("-cc") && i < argc-1)
			mainWindow->email->CCs.append(QString(argv[++i]));
		else if (QString(argv[i]) == QString("-autosave") && i < argc-1)
			mainWindow->enableAutosave(QString(argv[++i]), 10);
		else if (QString(argv[i])[0] != QChar('-'))
		{
			mainWindow->openFile(QString(argv[i]), !firstOpenRequest);
			firstOpenRequest = false;
		}
	}

	if (fullscreen)
		mainWindow->fullscreenViewActionTriggered();
	else	
		mainWindow->windowedViewActionTriggered();
}

bool VGApp::event(QEvent *event)
{
	// Double-clicking a .glass file to open it (OS X, others?)
	switch (event->type()) 
	{
		case QEvent::FileOpen:
			mainWindow->openFile(static_cast<QFileOpenEvent*>(event)->file(), !firstOpenRequest); 
			firstOpenRequest = false;
			return true;
		default:
			return QApplication::event(event);
	}
}

VGApp::~VGApp()
{
	// should probably free mainWindow...
}

