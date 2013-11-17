#ifndef GLASSCOLORLIBRARYWIDGET_H
#define GLASSCOLORLIBRARYWIDGET_H

#include "asyncrenderwidget.h"

class GlassColor;
class Cane;

class GlassColorLibraryWidget : public AsyncRenderWidget
{
	public:
		GlassColorLibraryWidget(GlassColor* _color, MainWindow* _window,
			Cane* _circlePlan=NULL, Cane* _squarePlan=NULL);
		const QPixmap *dragPixmap();
		bool isDraggable();
		GlassMime::Type mimeType();

		void updatePixmaps();
		virtual void paintEvent(QPaintEvent *); //to overlay color name
		GlassColor* glassColor;
		Cane* circlePlan;
		Cane* squarePlan;

	private:
		QPixmap _dragPixmap;
		void updateDragPixmap();
};

#endif
