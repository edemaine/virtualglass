

#ifndef PULLPLAN_H
#define PULLPLAN_H

class PullPlanLibraryWidget;

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "pulltemplate.h"

using std::vector;

class PullPlan;
class SubpullTemplate;

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

                vector<SubpullTemplate> subs;

		bool hasDependencyOn(Color* color);
		bool hasDependencyOn(PullPlan* pullPlan);

		void activate(int i);
		void deactivate(int i);
		bool isActivated(int i);

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

		vector<bool>* activated;

                // Methods
                void initializeTemplate();
                void updateSubs();
                void pushNewSubpull(vector<SubpullTemplate>* newSubs,
			int shape, Point location, float diameter, int group);
};

#endif

