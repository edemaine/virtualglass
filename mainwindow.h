#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "openglwidget.h"
#include "recipewidget.h"
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
	RecipeWidget* recipeWidget;
	QPushButton* pull_button;
	QPushButton* bundle_button;
	QPushButton* case_button;
	QPushButton* flatten_button;
	QPushButton* wrap_button;
	QPushButton* snap_button;
	QPushButton* undo_button;
	QPushButton* redo_button;
	QPushButton* save_button;
	QPushButton* clear_button;
	QPushButton* tabletop_button;

signals:
	void setCaneSig(Cane* c);
	void clearCommandSig();

public slots:
	void modeChanged(int mode);
	void saveCaneToLibrary();
	void exportLibraryDialog();
	void importLibraryDialog();
	void exportCaneDialog();
	void importCaneDialog();
	void newColorPickerCaneDialog();
	void changeBgColorDialog();
	void saveObjFileDialog();
	void saveRawFile();
	void colorPickerSelected();
	void libraryCaneDestroyed(QObject* obj);
	void loadLibraryCane(const YAML::Node& node, Cane* cane);
	void displayTextMessage(QString message,int msec);
	void displayTextMessage(QString message);
	void toggleRecipe();

private:
	Model* model;

	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);
	void setupLibraryArea();
	void setupRecipeArea();
	void setupStatusBar();
	void setupWorkArea();
	void setupMenuBar();
	void setupNewColorPickerCaneDialog();

	QMenu* caneMenu;
	QMenu* viewMenu;
	QMenu* fileMenu;
	QWidget* centralWidget;
	QVBoxLayout* windowLayout;
	QGraphicsView* workview;
	QGraphicsScene* workscene;
	QScrollArea* scrollArea;
	QScrollArea* libraryScrollArea;
	QHBoxLayout* stockLayout;
	QStackedLayout* stackLayout;
	QTabWidget* tabWidget;
	QLabel* imageLabel;
	QStatusBar* statusBar;
	QLabel* modeLabel;
	int librarySize;

	QDialog* caneDialog;
	QColorDialog* colorDialog;
	QFormLayout* caneForm;
	QComboBox* caneTypeBox;

	bool isRecipe;


};

#endif


