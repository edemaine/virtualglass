#ifndef QGRAPHICSHIGHLIGHTEFFECT_H
#define QGRAPHICSHIGHLIGHTEFFECT_H

#include <QtGui>
#include "dependancy.h" 

class QGraphicsHighlightEffect : public QGraphicsEffect
{
	Q_OBJECT

public:
	explicit QGraphicsHighlightEffect(QObject *parent = 0);
	virtual QRectF boundingRectFor(const QRectF &sourceRect) const;
	void setHighlightType(enum Dependancy d);

signals:
	void styleSheetString(QString string);

public slots:
	void setStyleSheet(bool enableBorder);

protected:
	virtual void draw( QPainter *painter ); 

private:
	enum Dependancy dependancy;	
};

#endif // QGRAPHICSHIGHLIGHTEFFECT_H
