#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QtGui>
#include <QApplication>
#include <QtGlobal>
#include "mainwindow.h"

class Controller : public QObject
{
	public:
		Controller(int argc = 0, char **argv = NULL);
		int startUp();

	private:
		MainWindow* mainWindow;
		QApplication* app;
};

#endif

