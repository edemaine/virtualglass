#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
	void seedLibrary();
	OpenGLWidget* openglWidget;
	QPushButton* look_button;
	QPushButton* zoom_in_button;
	QPushButton* zoom_out_button;
	QPushButton* frontView_button;
	QPushButton* sideView_button;
	QPushButton* topView_button;
	QPushButton* switchView_button;
	QPushButton* toggle_axes_button;
	QPushButton* toggle_grid_button;
	QPushButton* pull_button;
	QPushButton* bundle_button;
	QPushButton* flatten_button;
	QPushButton* wrap_button;
	QPushButton* undo_button;
	QPushButton* save_button;
	QPushButton* clear_button;
	QPushButton* exportLibrary_button;
	QPushButton* importLibrary_button;
	QPushButton* colorPicker_button;
	QPushButton* bgColorPicker_button;
	QPushButton* saveObj_button;

signals:
	void setCaneSig(Cane* c);
	void clearCommandSig();
	void textMessageSig(QString message);

public slots:
	void modeChanged(int mode);
	void saveCaneToLibrary();
	void exportLibraryButtonPressed();
	void importLibraryButtonPressed();
	void newColorPickerCaneButtonPressed();
	void changeBgColorButtonPressed();
	void saveObjButtonPressed();
	void colorPickerSelected(QColor color);
	void libraryCaneDestroyed(QObject* obj);
	void loadLibraryCane(const YAML::Node& node, Cane* cane);
	void displayTextMessage(QString message);

private:
	Model* model;

	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);
	void setupLibraryArea();
	void setupStatusBar();
	void setupWorkArea();

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


