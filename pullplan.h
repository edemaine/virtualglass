

#ifndef PULLPLAN_H
#define PULLPLAN_H

class PullPlanLibraryWidget;

#include <stdlib.h>
#include <vector>
#include "pulltemplate.h"

using std::vector;

class PullPlan;

class PullPlan
{
	public:
		PullPlan(int pullTemplate, Color* color);

		void setTemplateType(int pullTemplateType);
		int getTemplateType();
                bool isBase();

                void setParameter(int param, int newValue);
                int getParameter(int param);
                char* getParameterName(int param);
                unsigned int getParameterCount();

                void setShape(int s);
                int getShape();

                void setCasingThickness(float t);
                float getCasingThickness();

		void setTwist(float t);
		float getTwist();

		void setColor(Color* c);
		Color* getColor();

		PullPlan* copy();

		vector<PullPlan*> subplans;
                vector<SubpullTemplate> subtemps;

		bool hasDependencyOn(Color* color);
		bool hasDependencyOn(PullPlan* pullPlan);

	private:
		// Variables
		PullPlan* defaultCircleSubplan;
		PullPlan* defaultSquareSubplan;

		int templateType;
                int shape;
                float casingThickness;
		float twist;
		Color* color;
                vector<int> parameterValues;
                vector<char*> parameterNames;

                // Methods
                void initializeTemplate();
                void updateSubtemps();
                void updateSubplans();
};

#endif

