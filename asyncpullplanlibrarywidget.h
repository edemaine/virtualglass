#ifndef ASYNCPULLPLANLIBRARYWIDGET_H
#define ASYNCPULLPLANLIBRARYWIDGET_H

#include "asyncrenderwidget.h"
#include "pullplan.h"
#include "mesh.h"
#include "qgraphicshighlighteffect.h"
#include "pullplanrenderdata.h"

class AsyncPullPlanLibraryWidget : public AsyncRenderWidget
{
	public:
		AsyncPullPlanLibraryWidget(PullPlan *plan, QWidget * parent=NULL);
		PullPlan *getPullPlan();
		const QPixmap *getDragPixmap();
		void updatePixmaps();

	private:
		PullPlan *pullPlan;
		QPixmap dragPixmap;

		void updateDragPixmap();
                static void paintShape(float x, float y, float size, int shape, QPainter* painter);
                static void setBoundaryPainter(QPainter* painter, bool outermostLevel);
                static void drawSubplan(float x, float y, float drawWidth, float drawHeight,
                        PullPlan* plan, int mandatedShape, bool outermostLevel, QPainter* painter);
};

#endif //ASYNCPULLPLANLIBRARYWIDGET_H
