#ifndef WORKWINDOW_H
#define WORKWINDOW_H

#include <QtGui>
#include "openglwidget.h"
#include "constants.h"
#include "primitives.h"
#include "librarycanewidget.h"
#include "cane.h"
#include "yaml-cpp/yaml.h"

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
                void modeSelect(int index);
                void libraryCaneDestroyed(QObject* obj);

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
                QTabWidget* tabWidget;
                QLabel* imageLabel;
                int librarySize;
};
#endif


