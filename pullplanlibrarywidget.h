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
		const QPixmap *getDragPixmap();
		bool isDraggable();
		GlassMime::Type mimeType();
		void updatePixmaps();

	private:
		QPixmap dragPixmap;
		void updateDragPixmap();
};

#endif //PULLPLANLIBRARYWIDGET_H
