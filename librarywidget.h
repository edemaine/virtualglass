#ifndef LIBRARYWIDGET_H
#define LIBRARYWIDGET_H

#include <QtGui>
#include "dependancy.h" 

class LibraryWidget : public QLabel
{

public:
	LibraryWidget(QWidget* parent);
	void setBusy(bool isBusy);
	void setDependancy(bool hasDependancy, enum Dependancy _d=IS_DEPENDANCY);

protected:
	void updateStyleSheet();

private:
	bool hasDependancy;
	enum Dependancy dependancy;
	bool isBusy;
};

#endif // LIBRARYWIDGET_H
