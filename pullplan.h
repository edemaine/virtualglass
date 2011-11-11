

#ifndef PULLPLAN_H
#define PULLPLAN_H

class PullPlanLibraryWidget;

#include <stdlib.h>
#include <vector>
#include "pulltemplate.h"
#include <QPixmap>
#include "pullplanlibrarywidget.h"

class PullPlan;

class PullPlan
{
	public:
		PullPlan(int pullTemplate, bool isBase, Color color);

		void setTemplate(PullTemplate* pt);
		PullTemplate* getTemplate();

		void setLibraryWidget(PullPlanLibraryWidget* plplw);
		PullPlanLibraryWidget* getLibraryWidget();

		vector<PullPlan*> subplans;
		float twist;
		bool isBase; // base color pull plan
		Color color;
		const QPixmap* getEditorPixmap();
		void updatePixmap(QPixmap editorPixmap);

		Color getColorAverage();

	private:
		// Variables
		PullTemplate* pullTemplate;
		QPixmap editorPixmap;
		PullPlanLibraryWidget* libraryWidget;
};

#endif

