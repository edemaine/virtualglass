#ifndef GLASSCOLORLIBRARYWIDGET_H
#define GLASSCOLORLIBRARYWIDGET_H

#include "asyncrenderwidget.h"

class GlassColor;
class Cane;

class GlassColorLibraryWidget : public AsyncRenderWidget
{
	Q_OBJECT

	public:
		GlassColorLibraryWidget(GlassColor* _color, MainWindow* _window,
			Cane* _circleCane=NULL, Cane* _squareCane=NULL);
		const QPixmap *dragPixmap();
		bool isDraggable();
		GlassMime::Type mimeType();

		virtual void paintEvent(QPaintEvent *); //to overlay color name
		GlassColor* glassColor;
		Cane* circleCane;
		Cane* squareCane;

	public slots:
		void updatePixmaps();

	private:
		QPixmap _dragPixmap;
		void updateDragPixmap();
};

#endif
