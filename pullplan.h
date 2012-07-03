

#ifndef PULLPLAN_H
#define PULLPLAN_H

class PullPlanLibraryWidget;

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "pulltemplate.h"
#include "casing.h"
#include <QString>

using std::vector;

class PullPlan;
class SubpullTemplate;

class PullPlan
{
	public:
		PullPlan(int pullTemplate);

		void setTemplateType(int pullTemplateType);
		int getTemplateType();
		bool isBase();

		void setParameter(int param, int newValue);
		int getParameter(int param);
		char* getParameterName(int param);
		unsigned int getParameterCount();

		void setCasingThickness(float t, unsigned int index);
		void setOutermostCasingShape(int shape);
		void setCasingColor(Color* c, unsigned int index);
		void addCasing(int shape);
		void removeCasing();
		float getCasingThickness(unsigned int index);
		int getCasingShape(unsigned int index);
		int getOutermostCasingShape();
		void setOutermostCasingColor(Color* color);
		Color* getOutermostCasingColor();
		Color* getCasingColor(unsigned int index);
		unsigned int getCasingCount(); 
		bool hasSquareCasing();
		void setName(QString _name);
		QString getName();

		void setTwist(float t);
		float getTwist();

		PullPlan* copy() const;

		vector<SubpullTemplate> subs;

		bool hasDependencyOn(Color* color);
		bool hasDependencyOn(PullPlan* pullPlan);

	private:
		// Variables
		PullPlan* defaultCircleSubplan;
		PullPlan* defaultSquareSubplan;
		Color* defaultColor;

		int templateType;
		vector<Casing> casings;
		float twist;
		vector<int> parameterValues;
		vector<char*> parameterNames;
		QString name;

		// Methods
		void initializeTemplate();
		void updateSubs(vector<SubpullTemplate> oldSubs);
		void pushNewSubpull(vector<SubpullTemplate>* newSubs,
		int shape, Point location, float diameter, int group);
};

PullPlan *deep_copy(const PullPlan *plan);
void deep_delete(PullPlan *plan);

#endif

