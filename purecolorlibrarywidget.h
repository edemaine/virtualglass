#ifndef PURECOLORLIBRARYWIDGET_H
#define PURECOLORLIBRARYWIDGET_H

#include <QtGui>
#include "pullplan.h"

class PureColorLibraryWidget : public QLabel
{
	public:
		PureColorLibraryWidget(Color color, QString colorName, QWidget* parent=0);
		Color getColor();
		QString getColorName();
		void setAlpha(float a);
		void setSelected(bool s);

	private:
		void renderPixmap();
	
		Color color;
		QString colorName;
		bool isSelected;
};

#endif
