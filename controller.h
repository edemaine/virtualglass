#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QApplication>
#include "workwindow.h"

class Controller
{
        public:
                Controller();
                int run();

        protected:
                WorkWindow* workWindow;
                QApplication* app;
};

#endif

