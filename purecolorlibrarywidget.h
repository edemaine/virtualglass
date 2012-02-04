#ifndef PURECOLORLIBRARYWIDGET_H
#define PURECOLORLIBRARYWIDGET_H

#include <QtGui>
#include <QObject>
#include "pullplan.h"
#include "qgraphicshighlighteffect.h"

class PureColorLibraryWidget : public QLabel
{
	Q_OBJECT

	public:
		PureColorLibraryWidget(Color color, QString colorName, QWidget* parent=0);
		Color getColor();
		QString getColorName();
		void setAlpha(float a);

	private:
		Color color;
		QString colorName;
};

#endif
