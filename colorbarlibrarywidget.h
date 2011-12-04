#ifndef COLORBARLIBRARYWIDGET_H
#define COLORBARLIBRARYWIDGET_H

#include <QtGui>
#include <QObject>
#include "pullplan.h"

class ColorBarLibraryWidget : public QLabel
{
	Q_OBJECT

	public:
		ColorBarLibraryWidget(PullPlan* plan, QWidget* parent=0);
		PullPlan* getPullPlan();
		void updatePixmaps(QPixmap niceViewPixmap, QPixmap editorPixmap);
		void addPullPlan(PullPlan* plan);
		vector<PullPlan*> pullPlans;
		const QPixmap* getEditorPixmap();

	private:
		QButtonGroup* colorBarTemplateShapeButtonGroup;
		PullPlan* pullPlan;
		QPixmap editorPixmap;
};

#endif
