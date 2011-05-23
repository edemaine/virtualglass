#ifndef WORKWINDOW_H
#define WORKWINDOW_H

#include <QtGui>
#include "openglwidget.h"
#include "constants.h"
#include "librarycanewidget.h"
#include "cane.h"

class View;

class MainWindow : public QMainWindow
{
        Q_OBJECT

        public:
                MainWindow();
                void saveCaneToLibrary();
                void seedLibrary();
                
        public slots:
                void zoomInButtonPressed();
                void zoomOutButtonPressed();
                void lookButtonPressed();
                void twistButtonPressed();
                void stretchButtonPressed();
                void bundleButtonPressed();
                void squareoffButtonPressed();
                void nextButtonPressed();
                void saveButtonPressed();
                void clearButtonPressed();

        protected:
                void keyPressEvent(QKeyEvent* e);
                void setupLibraryArea();
                void setupWorkArea();

                OpenGLWidget* glassgl;
                QWidget* centralWidget;
                QVBoxLayout* windowLayout;
                QGraphicsView* workview;
                QGraphicsScene* workscene;
                QScrollArea* scrollArea;
                QScrollArea* libraryScrollArea;
                QHBoxLayout* stockLayout;
                QLabel* imageLabel;
                int librarySize;
};
#endif


