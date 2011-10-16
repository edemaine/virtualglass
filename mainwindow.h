

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QObject>
#include "niceviewwidget.h"
#include "pullplanlibrarywidget.h"
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
		QGraphicsScene* pullTemplateGraphicsScene;
		QLabel* niceViewLabel;
		QVBoxLayout* niceViewLayout;
		NiceViewWidget* niceViewWidget;
		QScrollArea* pullPlanLibraryScrollArea;
		QPushButton* savePullPlanButton; 
		QHBoxLayout* pullPlanLibraryLayout;

	private slots:
		void pullTemplateComboBoxChanged(int index);
		void savePullPlan();	
};


#endif

