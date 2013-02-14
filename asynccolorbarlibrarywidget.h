#ifndef ASYNCCOLORBARLIBRARYWIDGET_H
#define ASYNCCOLORBARLIBRARYWIDGET_H

#include "asyncrenderwidget.h"

class GlassColor;
class PullPlan;

class AsyncColorBarLibraryWidget : public AsyncRenderWidget
{
	public:
		AsyncColorBarLibraryWidget(GlassColor* _color, QWidget* _parent=NULL);
		const QPixmap *getDragPixmap();
		void updatePixmaps();
		virtual void paintEvent(QPaintEvent *); //to overlay color name
		GlassColor* glassColor;
		PullPlan* circlePlan;
		PullPlan* squarePlan;

	private:
		QPixmap dragPixmap;
		void updateDragPixmap();
};

#endif
