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
	Q_OBJECT

	public:
		PureColorLibraryWidget(GlassColor* _color, QWidget* parent);
		GlassColor* glassColorCopy();
		Color color();
		string shortName();
		string longName();
		void setSelected(bool s);

	signals:
		void colorSelected(GlassColor*);

	protected:
		void mousePressEvent(QMouseEvent* e);
		void mouseMoveEvent(QMouseEvent* e);
		void mouseReleaseEvent(QMouseEvent* e);

	private:
		bool clickDown;
		void renderPixmap();
		GlassColor* _color;
		bool isSelected;
		QLabel* swatch;
};

#endif
