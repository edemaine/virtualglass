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
		Controller();
		int startUp();

	signals:
		void modelChangedSig();

	public slots:
		void undoCommandSlot(); 

	protected:
		MainWindow* mainWindow;
		Model* model;
		OpenGLWidget* openglWidget;
		QApplication* app;
};

#endif

