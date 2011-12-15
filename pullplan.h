

#ifndef PULLPLAN_H
#define PULLPLAN_H

class PullPlanLibraryWidget;

#include <stdlib.h>
#include <vector>
#include "pulltemplate.h"
#include "pullplanlibrarywidget.h"
#include "colorbarlibrarywidget.h"

class PullPlan;

class PullPlan
{
	public:
		PullPlan();
		PullPlan(int pullTemplate, Color* color);

		void setTemplate(PullTemplate* pt);
		PullTemplate* getTemplate();

		void setLibraryWidget(PullPlanLibraryWidget* plplw);
		PullPlanLibraryWidget* getLibraryWidget();

		PullPlan* copy();

		vector<PullPlan*> subplans;
		float twist;
		Color* color;
		bool hasDependencyOn(Color* color);
		bool hasDependencyOn(PullPlan* pullPlan);

	private:
		// Variables
		PullTemplate* pullTemplate;
		PullPlanLibraryWidget* libraryWidget;
};

#endif

