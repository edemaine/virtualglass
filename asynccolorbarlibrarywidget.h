#ifndef ASYNCCOLORBARLIBRARYWIDGET_H
#define ASYNCCOLORBARLIBRARYWIDGET_H

#include "asyncrenderwidget.h"

class GlassColor;

class AsyncColorBarLibraryWidget : public AsyncRenderWidget
{
	public:
		AsyncColorBarLibraryWidget(GlassColor* _color, QWidget* _parent=NULL);
		GlassColor* getGlassColor();
		const QPixmap *getDragPixmap();
		void updatePixmaps();
		virtual void paintEvent(QPaintEvent *); //to overlay color name

	private:
		GlassColor* glassColor;
		QPixmap dragPixmap;

		void updateDragPixmap();
};

#endif
