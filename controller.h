#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QApplication>
#include <string.h>
#include "mainwindow.h"

class Controller
{
    public:
        Controller();
        int run();

    protected:
        MainWindow* mainWindow;
        QApplication* app;
};

#endif

