#ifndef GLASSLIBRARYWIDGET_H
#define GLASSLIBRARYWIDGET_H

#include <QPoint>
#include <QLabel>

#include "dependancy.h" 

class QWidget;
class QPaintEvent;
class QMouseEvent;
class MainWindow;

class GlassLibraryWidget : public QLabel
{
	public:
		GlassLibraryWidget(MainWindow* window);
		void setDependancy(enum Dependancy _d=IS_DEPENDANCY);
		virtual void paintEvent(QPaintEvent* event);		

	protected:
		void mousePressEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);

	private:
		enum Dependancy dependancy;
		void updateStyleSheet();
		int hitValue(QPoint hit);
		MainWindow* window;
		bool clickDown;
		QPoint clickDownPos;
};

#endif 
