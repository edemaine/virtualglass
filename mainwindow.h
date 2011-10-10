

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QObject>
#include "niceviewwidget.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow();

	private:
		// Methods
		void setupTable();
		void setupNiceView();	
		void setupPullPlanEditor();
		void setupConnections();

		// Variables
		QWidget* centralWidget;
		QHBoxLayout* centralLayout;
		QFormLayout* editorLayout;
		QLabel* editorLabel;
		QComboBox* pullTemplateComboBox;
		QLabel* niceViewLabel;
		QVBoxLayout* niceViewLayout;
		NiceViewWidget* niceViewWidget;
};


#endif

