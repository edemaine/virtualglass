#ifndef PULLPLANLIBRARYWIDGET_H
#define PULLPLANLIBRARYWIDGET_H

#include "asyncrenderwidget.h"
#include "shape.h"

class PullPlan;

class PullPlanLibraryWidget : public AsyncRenderWidget
{
	public:
		PullPlanLibraryWidget(PullPlan *_plan, MainWindow* _window);
		PullPlan *pullPlan;
		const QPixmap *dragPixmap();
		bool isDraggable();
		GlassMime::Type mimeType();
		void updatePixmaps();

	private:
		QPixmap _dragPixmap;
		void updateDragPixmap();
};

#endif //PULLPLANLIBRARYWIDGET_H
