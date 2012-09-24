#ifndef ASYNCCOLORBARLIBRARYWIDGET_H
#define ASYNCCOLORBARLIBRARYWIDGET_H

#include "pullplanrenderdata.h"
#include "asyncrenderwidget.h"
#include "pullplan.h"
#include "primitives.h"
#include "glasscolor.h"
#include "qgraphicshighlighteffect.h"
#include "shape.h"

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
