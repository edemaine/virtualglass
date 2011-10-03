

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QObject>

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow();

	private:
		// Methods
		void setupTable();
		void setupEditors();
		void setupNiceView();	

		// Variables
		QWidget* centralWidget;
};


#endif

