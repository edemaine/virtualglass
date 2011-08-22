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
	QPushButton* flatten_button;
	QPushButton* snap_button;
	QPushButton* undo_button;
	QPushButton* redo_button;
	QPushButton* save_button;
	QPushButton* clear_button;
	QPushButton* toggle2D_button;

signals:
	void setCaneSig(Cane* c);
	void clearCommandSig();
	void setNewMode(int i, bool viewRecipe,Cane* c);

public slots:
	void modeChanged(int mode);
	void projectionChanged();
	void saveCaneToLibrary();
	void exportLibraryDialog();
	void importLibraryDialog();
	void exportCaneDialog();
	void importCaneDialog();
	void newBrandCaneDialog();
	void newColorPickerCaneDialog();
	void changeBgColorDialog();
	void saveObjFileDialog();
	void saveRawFile();
	void colorPickerSelected();
	void colorBrandPickerSelected();
	void libraryCaneDestroyed(QObject* obj);
	void loadLibraryCane(const YAML::Node& node, Cane* cane);
	void displayTextMessage(QString message,int msec);
	void displayTextMessage(QString message);
	void updateSublist(QModelIndex i);
	void updateColor(QModelIndex i);
	void toggleRecipe();
	void updateLibraryToolTip(LibraryCaneWidget* lc);
	void caneChanged();
	void newMode(int i);
	void insertLibraryCane(Cane* c);

private:
	Model* model;

	void updateModeButtonsEnabled();
	void updatePreview();
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);
	void setupLibraryArea();
	void setupRecipeArea();
	void setupStatusBar();
	void setupWorkArea();
	void setupMenuBar();
	void setupNewColorPickerCaneDialog();
	void setupNewBrandCaneDialog();

	QPainter* makePainter(int caneType, int caneIndex);
	void loadOfficialCanes();

	QMenu* caneMenu;
	QMenu* viewMenu;
	QMenu* fileMenu;
	QAction* perspectiveProjection;
	QAction* orthographicProjection;

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

	QLabel* previewLabel;
	QDialog* caneDialog;
	QColorDialog* colorDialog;
	QFormLayout* caneForm;
	QComboBox* caneTypeBox;
	QSpinBox* verticesBox;
	QDoubleSpinBox* caneRadiusBox;

	QFormLayout* caneBrandForm;
	QComboBox* caneTypeBoxBrand;
	QSpinBox* verticesBoxBrand;
	QDoubleSpinBox* caneRadiusBoxBrand;

	bool isRecipe;
	QDialog* brandDialog;
	QSplitter* caneSplitter;
	QStringListModel* caneTypeListModel;
	QListView* caneTypeListBox;
	QStringList* dummyList;
	QStringListModel* dummyModel;
	bool dummyInUse;
	int selectedBrand;
	int selectedColor;
	QStringList* caneTypeList;
	//QTreeView* caneColorListBox;
	QListView* caneColorListBox;
	QList<QStringList>* caneNameListList;
	QList<QList<QColor> >* caneColorListList;


};

#endif


