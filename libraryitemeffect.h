#ifndef LIBRARYITEMEFFECT_H
#define LIBRARYITEMEFFECT_H

#include <QtGui>
#include "dependancy.h" 

class LibraryItemEffect : public QGraphicsEffect
{
	Q_OBJECT

public:
	explicit LibraryItemEffect(QObject *parent = 0);
	virtual QRectF boundingRectFor(const QRectF &sourceRect) const;
	void setBusy(bool isBusy);
	void setDependancy(bool hasDependancy, enum Dependancy _d=IS_DEPENDANCY);

signals:
	void styleSheetString(QString string);

protected:
	virtual void draw( QPainter *painter ); 

private:
	bool hasDependancy;
	enum Dependancy dependancy;
	bool isBusy;	

	void setStyleSheet();
};

#endif // LIBRARYITEMEFFECT_H
