#ifndef QGRAPHICSHIGHLIGHTEFFECT_H
#define QGRAPHICSHIGHLIGHTEFFECT_H

#include <QGraphicsEffect>
#include <QtGui>
#include "constants.h"

class QGraphicsHighlightEffect : public QGraphicsEffect
{
	Q_OBJECT

public:
	explicit QGraphicsHighlightEffect(QObject *parent = 0);
	virtual QRectF boundingRectFor(const QRectF &sourceRect) const;
	void setHighlightType(int dependancy);

signals:
	void styleSheetString(QString string);

public slots:
	void setStyleSheet(bool enableBorder);

protected:
	virtual void draw( QPainter *painter ); // , QGraphicsEffectSource *source );

private:
	int dependancy;	
};

#endif // QGRAPHICSHIGHLIGHTEFFECT_H
