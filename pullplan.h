

#ifndef PULLPLAN_H
#define PULLPLAN_H

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "templateparameter.h"
#include "subpulltemplate.h"
#include "pulltemplate.h"
#include "casing.h"
#include "glasscolor.h"

using std::vector;

class PullPlan;
class SubpullTemplate;

class PullPlan
{
	public:
		PullPlan(enum PullTemplate::Type pullTemplate);

		void setTemplateType(enum PullTemplate::Type pullTemplate, bool force=false);
		void setTemplateTypeToCustom();
		int getTemplateType();

		unsigned int getParameterCount();
                void getParameter(unsigned int index, TemplateParameter* dest);
                void setParameter(unsigned int index, int value);

		void setCasingThickness(float t, unsigned int index);
		void setOutermostCasingShape(int shape);
		void setCasingColor(GlassColor* gc, unsigned int index);
		void addCasing(int shape);
		void removeCasing();
		float getCasingThickness(unsigned int index);
		int getCasingShape(unsigned int index);
		int getOutermostCasingShape();
		void setOutermostCasingColor(GlassColor* gc);
		GlassColor* getOutermostCasingColor();
		GlassColor* getCasingColor(unsigned int index);
		unsigned int getCasingCount(); 
		bool hasSquareCasing();

		void setTwist(float t);
		float getTwist();

		PullPlan* copy() const;

		vector<SubpullTemplate> subs;

		bool hasDependencyOn(GlassColor* color);
		bool hasDependencyOn(PullPlan* pullPlan);

	private:
		// Variables
		PullPlan* defaultCircleSubplan;
		PullPlan* defaultSquareSubplan;
		GlassColor* defaultGlassColor;

		enum PullTemplate::Type templateType;
		vector<Casing> casings;
		float twist;
		vector<TemplateParameter> parameters;

		// Methods
		void initializeTemplate();
		void resetSubs(vector<SubpullTemplate> oldSubs);
		void pushNewSubpull(vector<SubpullTemplate>* newSubs,
			int shape, Point location, float diameter, int group);
};

PullPlan *deep_copy(const PullPlan *plan);
void deep_delete(PullPlan *plan);

#endif

