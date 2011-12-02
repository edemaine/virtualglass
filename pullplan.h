

#ifndef PULLPLAN_H
#define PULLPLAN_H

class PullPlanLibraryWidget;
class ColorBarLibraryWidget;

#include <stdlib.h>
#include <vector>
#include "pulltemplate.h"
#include <QPixmap>
#include "pullplanlibrarywidget.h"
#include "colorbarlibrarywidget.h"

class PullPlan;

class PullPlan
{
	public:
		PullPlan(int pullTemplate, Color* color);

		void setTemplate(PullTemplate* pt);
		PullTemplate* getTemplate();

		void setLibraryWidget(PullPlanLibraryWidget* plplw);
		void setColorLibraryWidget(ColorBarLibraryWidget* plplw);
		PullPlanLibraryWidget* getLibraryWidget();
		ColorBarLibraryWidget* getColorLibraryWidget();

		vector<PullPlan*> subplans;
		float twist;
		Color* color;
		const QPixmap* getEditorPixmap();
		void updatePixmap(QPixmap editorPixmap);

	private:
		// Variables
		PullTemplate* pullTemplate;
		QPixmap editorPixmap;
		PullPlanLibraryWidget* libraryWidget;
		ColorBarLibraryWidget* colorLibraryWidget;
};

#endif

