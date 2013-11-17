#ifndef CANELIBRARYWIDGET_H
#define CANELIBRARYWIDGET_H

#include "asyncrenderwidget.h"
#include "shape.h"

class Cane;

class CaneLibraryWidget : public AsyncRenderWidget
{
	public:
		CaneLibraryWidget(Cane *_plan, MainWindow* _window);
		Cane *pullPlan;
		const QPixmap *dragPixmap();
		bool isDraggable();
		GlassMime::Type mimeType();
		void updatePixmaps();

	private:
		QPixmap _dragPixmap;
		void updateDragPixmap();
};

#endif //PULLPLANLIBRARYWIDGET_H
