
#include "controller.h"

Controller::Controller()
{
        int one = 1;
        int& fake_argc = one;

        char** fake_argv;
        fake_argv = new char*[1];
        fake_argv[0] = new char[10];
        strcpy(fake_argv[0], "canelib");
        
        app = new QApplication(fake_argc, fake_argv);
        mainWindow = new MainWindow();
}

int Controller::run()
{
        mainWindow->show();
        mainWindow->seedLibrary();
        return app->exec();  
}

