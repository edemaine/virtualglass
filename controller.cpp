/*
The Controller class is a wrapper object for the
entire VirtualGlass problem. Creating a Controller
object and invoking Controller.run() starts the
application.
*/

#include "controller.h"

Controller::Controller()
{
	int one = 1;
	int& fake_argc = one;
	char** fake_argv;
	fake_argv = new char*[1];
	fake_argv[0] = new char[10];
	strcpy(fake_argv[0], "virtualglass");
	app = new QApplication(fake_argc, fake_argv);
	model = new Model();
	mainWindow = new MainWindow(model);

	// connect openglwidget to model
	connect(model, SIGNAL(caneChanged()), mainWindow->openglWidget, SLOT(caneChanged()));

	// Connect mainwindow to model
	connect(model, SIGNAL(textMessage(QString)), mainWindow, SLOT(displayTextMessage(QString)));
	connect(model, SIGNAL(modeChanged(int)), mainWindow, SLOT(modeChanged(int)));

	// connect mainwindow buttons to model
	QSignalMapper* modeSignalMapper = new QSignalMapper(model);
	modeSignalMapper->setMapping(mainWindow->pull_button, PULL_MODE);
	modeSignalMapper->setMapping(mainWindow->bundle_button, BUNDLE_MODE);
	modeSignalMapper->setMapping(mainWindow->flatten_button, FLATTEN_MODE);
	modeSignalMapper->setMapping(mainWindow->wrap_button, WRAP_MODE);
	connect(mainWindow->pull_button, SIGNAL(clicked()), modeSignalMapper, SLOT(map()));
	connect(mainWindow->bundle_button, SIGNAL(clicked()), modeSignalMapper, SLOT(map()));
	connect(mainWindow->flatten_button, SIGNAL(clicked()), modeSignalMapper, SLOT(map()));
	connect(mainWindow->wrap_button, SIGNAL(clicked()), modeSignalMapper, SLOT(map()));
	connect(modeSignalMapper, SIGNAL(mapped(int)), model, SLOT(setMode(int)));

	connect(mainWindow->undo_button, SIGNAL(pressed()), model, SLOT(undo()));
	connect(mainWindow->clear_button, SIGNAL(pressed()), model, SLOT(clearCurrentCane()));

	// Connect mainwindow buttons to mainwindow
	connect(mainWindow->save_button, SIGNAL(pressed()), mainWindow, SLOT(saveCaneToLibrary()));
	connect(mainWindow->exportLibrary_button, SIGNAL(pressed()),
			mainWindow, SLOT(exportLibraryButtonPressed()));
	connect(mainWindow->importLibrary_button, SIGNAL(pressed()),
			mainWindow, SLOT(importLibraryButtonPressed()));
	connect(mainWindow->colorPicker_button, SIGNAL(pressed()),
			mainWindow, SLOT(newColorPickerCaneButtonPressed()));
	connect(mainWindow->bgColorPicker_button, SIGNAL(pressed()),
			mainWindow, SLOT(changeBgColorButtonPressed()));
	connect(mainWindow->saveObj_button, SIGNAL(pressed()), mainWindow, SLOT(saveObjButtonPressed()));

	// connect openglwidget to mainwindow buttons
	connect(mainWindow->toggle_axes_button, SIGNAL(pressed()),
			mainWindow->openglWidget, SLOT(toggleAxes()));
	connect(mainWindow->toggle_grid_button, SIGNAL(pressed()),
			mainWindow->openglWidget, SLOT(toggleGrid()));
	connect(mainWindow->zoom_in_button, SIGNAL(pressed()),
			mainWindow->openglWidget, SLOT(zoomIn()));
	connect(mainWindow->zoom_out_button, SIGNAL(pressed()),
			mainWindow->openglWidget, SLOT(zoomOut()));
	connect(mainWindow->frontView_button, SIGNAL(pressed()),
			mainWindow->openglWidget, SLOT(setFrontView()));
	connect(mainWindow->topView_button, SIGNAL(pressed()),
			mainWindow->openglWidget, SLOT(setTopView()));
	connect(mainWindow->sideView_button, SIGNAL(pressed()),
			mainWindow->openglWidget, SLOT(setSideView()));
	connect(mainWindow->switchView_button, SIGNAL(pressed()),
			mainWindow->openglWidget, SLOT(switchProjection()));
}

int Controller::startUp()
{
	mainWindow->showMaximized();
	mainWindow->seedLibrary();
	model->setMode(LOOK_MODE);
	return app->exec();
}





