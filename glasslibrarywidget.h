#ifndef GLASSLIBRARYWIDGET_H
#define GLASSLIBRARYWIDGET_H

#include <QLabel>
#include "dependancy.h" 

class QWidget;

class GlassLibraryWidget : public QLabel
{
	public:
		GlassLibraryWidget(QWidget* parent);
		void setDependancy(bool hasDependancy, enum Dependancy _d=IS_DEPENDANCY);

	protected:
		void updateStyleSheet();

	private:
		bool hasDependancy;
		enum Dependancy dependancy;
};

#endif 
