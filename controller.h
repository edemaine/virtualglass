#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QApplication>
#include <string.h>
#include <QObject>
#include "openglwidget.h"
#include "mainwindow.h"
#include "model.h"

class Controller : public QObject
{
	Q_OBJECT

public:
	Controller(int argc = 0, char **argv = NULL);
	int startUp();

protected:
	MainWindow* mainWindow;
	Model* model;
	QApplication* app;
};

#endif

