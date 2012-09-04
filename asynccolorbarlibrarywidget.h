#ifndef ASYNCCOLORBARLIBRARYWIDGET_H
#define ASYNCCOLORBARLIBRARYWIDGET_H

#include "pullplanrenderdata.h"
#include "asyncrenderwidget.h"
#include "pullplan.h"
#include "primitives.h"
#include "glasscolor.h"
#include "qgraphicshighlighteffect.h"

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
                static void paintShape(float x, float y, float size, int shape, QPainter* painter);
                static void setBoundaryPainter(QPainter* painter, bool outermostLevel);
                static void drawSubplan(float x, float y, float drawWidth, float drawHeight,
                        PullPlan* plan, int mandatedShape, bool outermostLevel, QPainter* painter);
};

#endif
