#ifndef WORKWINDOW_H
#define WORKWINDOW_H

#include <QtGui>
#include "openglwidget.h"
#include "constants.h"
#include "primitives.h"
#include "librarycanewidget.h"
#include "cane.h"
#include "yaml-cpp/yaml.h"
#include "model.h"

class View;

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow(Model* model);
		void saveCaneToLibrary();
		void seedLibrary();
		OpenGLWidget* getOpenGLWidget();

	signals:
		void modeChangedCommandSig(int mode);
		void zoomInCommandSig();
		void zoomOutCommandSig();
                void undoCommandSig();
                void redoCommandSig();
		void toggleAxesCommandSig();
		void toggleGridCommandSig();
		void switchViewCommandSig();
		void advanceActiveCaneCommandSig();
		void saveCommandSig();
		void clearCommandSig();
		void textMessageSig(QString message);

	public slots:
		void modeChangedSlot(int mode);
		void saveCommandSlot();
		void exportLibraryButtonPressed();
		void importLibraryButtonPressed();
		void newColorPickerCaneButtonPressed();
		void changeBgColorButtonPressed();
		void saveObjButtonPressed();
		void colorPickerSelected(QColor color);
		void libraryCaneDestroyed(QObject* obj);
		void loadLibraryCane(const YAML::Node& node, Cane* cane);
		void textMessageSlot(QString message);

	private:
		Model* model;

		void keyPressEvent(QKeyEvent* e);
		void keyReleaseEvent(QKeyEvent* e);
		void setupLibraryArea();
		void setupStatusBar();
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
		QStatusBar* statusBar;
		QLabel* modeLabel;
		int librarySize;


};
#endif


