#ifndef ASYNCCOLORBARLIBRARYWIDGET_H
#define ASYNCCOLORBARLIBRARYWIDGET_H

#include "asyncpullplanlibrarywidget.h"

class AsyncColorBarLibraryWidget : public AsyncPullPlanLibraryWidget
{
	Q_OBJECT

	public:
		AsyncColorBarLibraryWidget(PullPlan* _plan, QWidget* _parent=NULL);
		void updateDragPixmap();
		void updatePixmaps();

		//to overlay color name:
		virtual void paintEvent(QPaintEvent *);
};

#endif
