#ifndef ASYNCPULLPLANLIBRARYWIDGET_H
#define ASYNCPULLPLANLIBRARYWIDGET_H

#include "asyncrenderwidget.h"
#include "pullplan.h"

class AsyncPullPlanLibraryWidget : public AsyncRenderWidget
{
	Q_OBJECT
	public:
		AsyncPullPlanLibraryWidget(PullPlan *plan, QWidget * parent=NULL);
		PullPlan *getPullPlan();
		const QPixmap *getEditorPixmap();
		//other pixmap will be rendered asynchronously from copy of current pull plan (one hopes?)
		void updatePixmaps();
	protected:
		PullPlan *pullPlan;
		QPixmap editorPixmap;
};

#endif //ASYNCPULLPLANLIBRARYWIDGET_H
