#ifndef WORKWINDOW_H
#define WORKWINDOW_H

#include <QtGui>
#include "cane.h"
#include "openglwidget.h"

class View;

class WorkWindow : public QMainWindow
{
        Q_OBJECT

        public:
                WorkWindow();
                void saveCaneToLibrary();
                void seedLibrary();
                
        public slots:
                void zoomInButtonPressed();
                void zoomOutButtonPressed();
                void lookButtonPressed();
                void twistButtonPressed();
                void stretchButtonPressed();
                void bundleButtonPressed();
                void nextButtonPressed();
                void saveButtonPressed();
                void clearButtonPressed();

        protected:
                void keyPressEvent(QKeyEvent* e);
                void setupLibraryArea();
                void setupWorkArea();

                OpenGLWidget* glassgl;
                QWidget* centralWidget;
                QVBoxLayout* window_layout;
                QGraphicsView* workview;
                QGraphicsScene* workscene;
                QScrollArea* scrollArea;
                QScrollArea* libraryScrollArea;
                QHBoxLayout* stock_layout;
                QLabel* imageLabel;
                int librarySize;
};
#endif


