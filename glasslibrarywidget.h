#ifndef GLASSLIBRARYWIDGET_H
#define GLASSLIBRARYWIDGET_H

#include <QLabel>
#include "dependancy.h" 

class QWidget;

class GlassLibraryWidget : public QLabel
{
	public:
		GlassLibraryWidget(QWidget* parent);
		void setBusy(bool isBusy);
		void setDependancy(bool hasDependancy, enum Dependancy _d=IS_DEPENDANCY);

	protected:
		void updateStyleSheet();

	private:
		bool hasDependancy;
		enum Dependancy dependancy;
		bool isBusy;
};

#endif 
