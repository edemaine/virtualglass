
#ifndef VGAPP_H
#define VGAPP_H

#include <QtGui>
#include "mainwindow.h"

class VGApp : public QApplication
{
	Q_OBJECT

	public:
		VGApp(int & argc, char **argv);
		virtual ~VGApp();

	protected:
		bool event(QEvent *);

	private:
		MainWindow *mainWindow;
		bool firstOpenRequest;	
};
#endif

