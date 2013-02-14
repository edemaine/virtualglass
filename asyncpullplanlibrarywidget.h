#ifndef ASYNCPULLPLANLIBRARYWIDGET_H
#define ASYNCPULLPLANLIBRARYWIDGET_H

#include "asyncrenderwidget.h"
#include "shape.h"

class PullPlan;

class AsyncPullPlanLibraryWidget : public AsyncRenderWidget
{
	public:
		AsyncPullPlanLibraryWidget(PullPlan *plan, QWidget * parent=NULL);
		PullPlan *pullPlan;
		const QPixmap *getDragPixmap();
		void updatePixmaps();

	private:
		QPixmap dragPixmap;

		void updateDragPixmap();
		static void paintShape(float x, float y, float size, enum GeometricShape shape, QPainter* painter);
		static void setBoundaryPainter(QPainter* painter, bool outermostLevel);
		static void drawSubplan(float x, float y, float drawWidth, float drawHeight,
			PullPlan* plan, bool outermostLevel, QPainter* painter);
};

#endif //ASYNCPULLPLANLIBRARYWIDGET_H
