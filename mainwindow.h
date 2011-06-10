#ifndef WORKWINDOW_H
#define WORKWINDOW_H

#include <QtGui>
#include "openglwidget.h"
#include "constants.h"
#include "primitives.h"
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
                void toggleAxesButtonPressed();
                void lookButtonPressed();
                void topViewButtonPressed();
                void sideViewButtonPressed();
                void twistButtonPressed();
                void stretchButtonPressed();
                void bundleButtonPressed();
                void flattenButtonPressed();
                void nextButtonPressed();
                void saveButtonPressed();
                void clearButtonPressed();
                void exportLibraryButtonPressed();
                void importLibraryButtonPressed();
                void newColorPickerCaneButtonPressed();
                void colorPickerSelected(QColor color);

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
                QColorDialog* colorPickerDialog;
                int librarySize;
};
#endif


