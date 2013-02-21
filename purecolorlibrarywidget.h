#ifndef PURECOLORLIBRARYWIDGET_H
#define PURECOLORLIBRARYWIDGET_H

#include <QLabel>
#include <string>
#include "primitives.h"

class GlassColor;

using std::string;

class PureColorLibraryWidget : public QLabel
{
	public:
		PureColorLibraryWidget(GlassColor* color, QWidget* parent=0);
		GlassColor* getGlassColorCopy();
		Color getColor();
		string getShortName();
		string getLongName();
		void setAlpha(float a);
		void setSelected(bool s);

	private:
		void renderPixmap();
		GlassColor* color;
		bool isSelected;
};

#endif
