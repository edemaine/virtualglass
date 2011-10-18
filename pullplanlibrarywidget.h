#ifndef PULLPLANLIBRARYWIDGET_H
#define PULLPLANLIBRARYWIDGET_H

#include <QtGui>
#include <QObject>
#include "pullplan.h"
#include "niceviewwidget.h"

class PullPlanLibraryWidget : public QLabel
{
	Q_OBJECT

	public:
		PullPlanLibraryWidget(QPixmap niceViewPixmap, QPixmap editorPixmap, PullPlan* plan, QWidget* parent=0);
		PullPlan* getPullPlan();
		const QPixmap* getEditorPixmap();
		void updatePixmaps(QPixmap niceViewPixmap, QPixmap editorPixmap);

	private:
		PullPlan* pullPlan;
		QPixmap editorPixmap;
};

#endif
