

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QObject>
#include "niceviewwidget.h"
#include "pullplanlibrarywidget.h"
#include "pulltemplate.h"
#include "model.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow(Model* model);

	private:
		// Methods
		void setupTable();
		void setupNiceView();	
		void setupPullPlanEditor();
		void setupConnections();
		void loadPullTemplate(PullTemplate* pt);

		// Variables
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
		void pullTemplateChanged(int index);
		void savePullPlan();	
};


#endif

