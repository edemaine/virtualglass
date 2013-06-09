#ifndef GLASSLIBRARYWIDGET_H
#define GLASSLIBRARYWIDGET_H

#include <QLabel>
#include "dependancy.h" 

class QWidget;

class GlassLibraryWidget : public QLabel
{
	public:
		GlassLibraryWidget(QWidget* parent);
		void setDependancy(enum Dependancy _d=IS_DEPENDANCY);

	protected:
		void updateStyleSheet();

	private:
		enum Dependancy dependancy;
};

#endif 
