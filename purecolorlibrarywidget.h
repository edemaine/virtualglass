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
		PureColorLibraryWidget(GlassColor* _color, QWidget* parent=0);
		GlassColor* glassColorCopy();
		Color color();
		string shortName();
		string longName();
		void setSelected(bool s);

	private:
		void renderPixmap();
		GlassColor* _color;
		bool isSelected;
};

#endif
