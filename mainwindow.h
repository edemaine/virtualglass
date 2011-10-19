

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QObject>
#include "constants.h"
#include "niceviewwidget.h"
#include "pullplanlibrarywidget.h"
#include "colorbarlibrarywidget.h"
#include "pulltemplate.h"
#include "model.h"
#include "pullplan.h"
#include "pullplaneditorviewwidget.h"
#include "pickupplan.h"
#include "pickupplaneditorviewwidget.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow(Model* model);
        	void mousePressEvent(QMouseEvent* event);
        	void dragMoveEvent(QDragMoveEvent* event);
		void seedTable();
	
	signals:
		void someDataChanged();

	private:
		// Methods
		void setupTable();
		void setupNiceView();	
		void setupEditors();
		void setupPullPlanEditor();
		void setupPickupPlanEditor();
		void setupConnections();
		void updatePullPlanEditor();
		void updateNiceView();
		void updateLibrary();

		// Variables
		QTabWidget* editorTabs;
		QWidget* pullPlanEditorPage;
		QWidget* pickupPlanEditorPage;
		PullPlan* pullPlanEditorPlan;
		PickupPlan* pickupPlanEditorPlan;
		Color defaultColor;
		PullPlanLibraryWidget* pullPlanEditorPlanLibraryWidget;
		Model* model;
		QWidget* centralWidget;
		QHBoxLayout* centralLayout;
		QComboBox* pullTemplateComboBox;
		QComboBox* pickupTemplateComboBox;
		PullPlanEditorViewWidget* pullPlanEditorViewWidget;
		PickupPlanEditorViewWidget* pickupPlanEditorViewWidget;
		QLabel* niceViewLabel;
		QVBoxLayout* niceViewLayout;
		NiceViewWidget* niceViewWidget;
		QScrollArea* pullPlanLibraryScrollArea;
		QScrollArea* colorBarLibraryScrollArea;
		QHBoxLayout* pullPlanLibraryLayout;
		QHBoxLayout* colorBarLibraryLayout;
		QSlider* pullPlanTwistSlider;
		QSpinBox* pullPlanTwistSpin;
		QPushButton* newPullPlanButton;

	private slots:
		void updateEverything();
		void pullTemplateComboBoxChanged(int index);
		void pickupTemplateComboBoxChanged(int index);
		void newPullPlan();	
		void pullPlanTwistSliderChanged(int p);
		void pullPlanTwistSpinChanged(int p);
};


#endif

