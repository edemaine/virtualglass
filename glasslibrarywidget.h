#ifndef GLASSLIBRARYWIDGET_H
#define GLASSLIBRARYWIDGET_H

#include <QPoint>
#include <QLabel>

#include "dependancy.h" 

class QWidget;
class QPaintEvent;
class QMouseEvent;

class GlassLibraryWidget : public QLabel
{
	public:
		GlassLibraryWidget(QWidget* parent);
		void setDependancy(enum Dependancy _d=IS_DEPENDANCY);
		virtual void paintEvent(QPaintEvent* event);		
		void mouseReleaseEvent(QMouseEvent* event);

	protected:
		enum Dependancy dependancy;
		void updateStyleSheet();
		int hitValue(QPoint hit);
};

#endif 
