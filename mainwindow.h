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

class KeyQListView : public QListView
{
public:
        KeyQListView(QWidget * parent) : QListView(parent) {}
        KeyQListView() : QListView() {}
protected:
        void keyPressEvent(QKeyEvent *event)
        {
                QModelIndex oldIdx = currentIndex();
                QListView::keyPressEvent(event);
                QModelIndex newIdx = currentIndex();
                if(oldIdx.row() != newIdx.row())
                {
                        emit clicked(newIdx);
                }
        }
};

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
	void shapeChangeRequest(int subcane);
	void colorChangeCustomRequest(int subcane);
	void colorChangeBrandRequest(int subcane);
	void setSubcaneColorFromPicker(QColor);
	void setSubcaneShapeFromPicker(QString s);
	void caneChanged();
	void modeChanged(int mode);
	void projectionChanged();
	void saveCaneToLibrary();
	void exportLibraryDialog();
	void importLibraryDialog();
	void exportCaneDialog();
	void importCaneDialog();
	void newBrandCaneDialog();
	void changeBgColorDialog();
	void saveObjFileDialog();
	void saveRawFile();
	void libraryCaneDestroyed(QObject* obj);
	void loadLibraryCane(const YAML::Node& node, Cane* cane);
	void displayTextMessage(QString message,int msec);
	void displayTextMessage(QString message);
	void updateBrandColorPickerSublist(QModelIndex i);
	void updateBrandColorPickerColor(QModelIndex i);
	void toggleRecipe();
	void updateLibraryToolTip(LibraryCaneWidget* lc);
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
	void setupShapeChangeDialog();
	void setupCustomColorChangeDialog();
	void setupBrandColorChangeDialog();

//	QStringListModel* dummyModel;
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
	QColorDialog* caneColorChangeColorPicker;
	int caneChangeSubcane;
	QDialog* brandDialog;
	QSplitter* caneSplitter;
	QStringListModel* caneTypeListModel;
        KeyQListView* caneTypeListBox;
	QStringList* dummyList;
	QStringListModel* dummyModel;
	bool dummyInUse;
	int selectedBrand;
	int selectedColor;
	QStringList* caneTypeList;
        //QTreeView* caneColorListBox;
        KeyQListView* caneColorListBox;
	QList<QStringList>* caneNameListList;
	QList<QList<QColor> >* caneColorListList;
	QDialog* shapeDialog;

	bool isRecipe;


};

#endif


