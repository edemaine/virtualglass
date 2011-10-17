

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QObject>
#include "niceviewwidget.h"
#include "pullplanlibrarywidget.h"
#include "colorbarlibrarywidget.h"
#include "pulltemplate.h"
#include "model.h"
#include "pullplan.h"
#include "pulltemplategraphicsview.h"

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
		void setupPullPlanEditor();
		void setupConnections();
		void updatePullPlanEditor();
		void updateNiceView();

		// Variables
		PullPlan* pullPlanEditorPlan;
		PullPlan* defaultPullPlanEditorPlan;
		Model* model;
		QWidget* centralWidget;
		QHBoxLayout* centralLayout;
		QComboBox* pullTemplateComboBox;
		PullTemplateGraphicsView* pullTemplateGraphicsView;
		QLabel* niceViewLabel;
		QVBoxLayout* niceViewLayout;
		NiceViewWidget* niceViewWidget;
		QScrollArea* pullPlanLibraryScrollArea;
		QScrollArea* colorBarLibraryScrollArea;
		QPushButton* savePullPlanButton; 
		QHBoxLayout* pullPlanLibraryLayout;
		QHBoxLayout* colorBarLibraryLayout;
		QSlider* pullPlanTwistSlider;
		QDoubleSpinBox* pullPlanTwistSpin;

	private slots:
		void updateEverything();
		void pullTemplateComboBoxChanged(int index);
		void savePullPlan();	
		void pullPlanTwistSliderChanged(int p);
		void pullPlanTwistSpinChanged(double v);
};


#endif

