#ifndef PURECOLORLIBRARYWIDGET_H
#define PURECOLORLIBRARYWIDGET_H

#include <QLabel>

#include <string>

#include "primitives.h"
#include "coloreditorwidget.h"

class GlassColor;

using std::string;

class PureColorLibraryWidget : public QLabel
{
	public:
		PureColorLibraryWidget(GlassColor* _color, QWidget* parent);
		GlassColor* glassColorCopy();
		Color color();
		string shortName();
		string longName();
		void setSelected(bool s);

	private:
		bool isSelected;

		void renderPixmap();
		GlassColor* _color;
		QLabel* swatch;
};

#endif
