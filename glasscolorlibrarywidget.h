#ifndef GLASSCOLORLIBRARYWIDGET_H
#define GLASSCOLORLIBRARYWIDGET_H

#include "asyncrenderwidget.h"

class GlassColor;
class PullPlan;

class GlassColorLibraryWidget : public AsyncRenderWidget
{
	public:
		GlassColorLibraryWidget(GlassColor* _color, QWidget* _parent=NULL,
			PullPlan* _circlePlan=NULL, PullPlan* _squarePlan=NULL);
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
