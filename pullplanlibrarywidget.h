#ifndef PULLPLANLIBRARYWIDGET_H
#define PULLPLANLIBRARYWIDGET_H

class PullPlan;

#include <QtGui>
#include <QObject>
#include "pullplan.h"

class PullPlanLibraryWidget : public QLabel
{
	Q_OBJECT

	public:
		PullPlanLibraryWidget(PullPlan* plan, QWidget* parent=0);
		PullPlan* getPullPlan();
		const QPixmap* getEditorPixmap();
		void updatePixmaps(QPixmap niceViewPixmap, QPixmap editorPixmap);

	private:
		PullPlan* pullPlan;
		QPixmap editorPixmap;
};

#endif
