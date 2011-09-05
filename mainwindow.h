#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "openglwidget.h"
#include "recipewidget.h"
#include "constants.h"
#include "primitives.h"
#include "librarycanewidget.h"
#include "cane.h"
#include "model.h"
#include "keyqlistview.h"
#include "canefile.h"

class View;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(Model* model);
	void seedLibrary();
	OpenGLWidget* openglWidget;
	RecipeWidget* recipeWidget;
		QPushButton* cane_button;
	QPushButton* pull_button;
	QPushButton* bundle_button;
	QPushButton* flatten_button;
	QPushButton* undo_button;
	QPushButton* redo_button;
	QPushButton* save_button;
	QPushButton* clear_button;
	QPushButton* toggle2D_button;

signals:
	void setNewMode(int i, bool viewRecipe,Cane* c);
		void x_locationChanged(float f);
		void y_locationChanged(float f);

public slots:
	void arrangementRequest(Cane* c);
	void addNewDefaultCane();
	void caneChangeRequest(int subcane);
	void geometryHeightEvent(int);
	void shapeTypeEvent(int);
	void shapeSizeEvent(int);
	void changeAlphaEvent(int i);
	void caneChanged();
	void modeChanged(int mode);
	void projectionChanged();
	void saveCaneToLibrary();
	void exportLibraryDialog();
	void importLibraryDialog();
	void exportCaneDialog();
	void newBrandCaneDialog();
	void changeBgColorDialog();
	void loadSVGFileDialog();
	void saveObjFileDialog();
	void saveRawFile();
	void libraryCaneDestroyed(QObject* obj);
	void displayTextMessage(QString message,int msec);
	void displayTextMessage(QString message);
	void updateBrandColorPickerSublist(QModelIndex i);
        void updateBrandColorPickerSublist(int index);
	void updateBrandColorPickerColor(QModelIndex i);
        void updateBrandColorPickerColor(int i);
	void toggleRecipe();
	void updateLibraryToolTip(LibraryCaneWidget* lc);
	void newMode(int i);
	void insertLibraryCane(Cane* c);
	void toggleFlat();

		void size_changeEditSliderFromText();
		void size_changeEditTextFromSlider(int i);
		void disconnectLocationSignals();
		void reconnectLocationSignals();
		void x_changeEditSliderFromText();
		void x_changeEditTextFromSlider(int i);
		void y_changeEditSliderFromText();
		void y_changeEditTextFromSlider(int i);
		void radius_changeEditSliderFromText();
		void radius_changeEditTextFromSlider(int i);
		void theta_changeEditSliderFromText();
		void theta_changeEditTextFromSlider(int i);
		void changeRThetaFromXY();
		void changeXYFromRTheta();

private:
	Model* model;

	void saveCaneColorAndShape();
	void revertCaneColorAndShape();
	void shapePickerEvent();
	void updateModeButtonsEnabled();
	void updatePreview();
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);
	void setupButtonBar();
	void setupRecipeArea();
	void setupOGLArea();
	void setupLibraryArea();
	void setupStatusBar();
	void setupWorkArea();
	void setupMenuBar();
	void setupChangeDialog();
	void setupArrangementDialog();
		void checkButton(int mode);
		void updateCaneAlphaSlider(int i);

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
	QVBoxLayout* operButton_layout;
	QWidget* casLayoutWidget;
	QWidget* oglLayoutWidget;
	QSlider* oglGeometryHeightSlider;
	QSlider* arrangementRadiusSlider;
	QSlider* arrangementCountSlider;
	QBoxLayout* stockLayout;
	QStackedLayout* stackLayout;
	QTabWidget* tabWidget;
	QLabel* imageLabel;
	QStatusBar* statusBar;
	QLabel* modeLabel;
	int librarySize;

	QLabel* previewLabel;
	QLabel* hiddenLabel;
	int caneChangeSubcane;
	Color savedColor;
	CaneShape savedShape;
		Point savedLocation;
        QString savedBrand;
	QDialog* changeDialog;
	QDialog* arrangementDialog;
	QSplitter* caneSplitter;
	QStringListModel* caneTypeListModel;
		KeyQListView* caneTypeListBox;
	QStringList* dummyList;
	QStringListModel* dummyModel;
	bool dummyInUse;
	int selectedBrand;
	int selectedColor;
	QStringList* caneTypeList;
	KeyQListView* caneColorListBox;
	QList<QStringList>* caneNameListList;
	QList<QList<QColor> >* caneColorListList;
	QComboBox* caneShapeBox;
		QSlider* caneAlphaSlider;
	QSlider* caneSizeSlider;
		QLineEdit* caneSizeEditBox;
	QSlider* caneHeightSlider;

		int xySliderResolution;
		int radiusSliderResolution;
		int thetaSliderResolution;
		int radiusMaxByDefault;
		QLabel* x_editlabel;
		QLineEdit* x_editbox;
		QSlider* x_editslider;
		QLabel* y_editlabel;
		QLineEdit* y_editbox;
		QSlider* y_editslider;
		QLabel* radius_editlabel;
		QLineEdit* radius_editbox;
		QSlider* radius_editslider;
		QLabel* theta_editlabel;
		QLineEdit* theta_editbox;
		QSlider* theta_editslider;

	bool isRecipe;

private slots:
	void toggleShow();
	void cancelCaneChangeDialog();
};

#endif


