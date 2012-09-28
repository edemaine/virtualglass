#ifndef LIBRARYITEM_H
#define LIBRARYITEM_H

#include <QtGui>
#include "dependancy.h" 

class LibraryItem : public QLabel
{

public:
	LibraryItem(QWidget* parent);
	void setBusy(bool isBusy);
	void setDependancy(bool hasDependancy, enum Dependancy _d=IS_DEPENDANCY);

protected:
	void updateStyleSheet();

private:
	bool hasDependancy;
	enum Dependancy dependancy;
	bool isBusy;
};

#endif // LIBRARYITEM_H
