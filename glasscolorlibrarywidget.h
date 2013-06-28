#ifndef GLASSCOLORLIBRARYWIDGET_H
#define GLASSCOLORLIBRARYWIDGET_H

#include "asyncrenderwidget.h"

class GlassColor;
class PullPlan;

class GlassColorLibraryWidget : public AsyncRenderWidget
{
	public:
		GlassColorLibraryWidget(GlassColor* _color, MainWindow* _window,
			PullPlan* _circlePlan=NULL, PullPlan* _squarePlan=NULL);
		const QPixmap *dragPixmap();
		bool isDraggable();
		GlassMime::Type mimeType();

		void updatePixmaps();
		virtual void paintEvent(QPaintEvent *); //to overlay color name
		GlassColor* glassColor;
		PullPlan* circlePlan;
		PullPlan* squarePlan;

	private:
		QPixmap _dragPixmap;
		void updateDragPixmap();
};

#endif
