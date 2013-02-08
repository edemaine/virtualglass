

#ifndef PULLPLAN_H
#define PULLPLAN_H

#include "pulltemplate.h"
#include "shape.h"
#include "primitives.h"
#include "subpulltemplate.h"
#include "casing.h"
#include <vector>

using std::vector;

class PullPlan;
class TemplateParameter;
class GlassColor;

class PullPlan
{
	public:
		PullPlan(enum PullTemplate::Type t);

		void setTemplateType(enum PullTemplate::Type t, bool force=false);
		enum PullTemplate::Type getTemplateType();

		unsigned int getParameterCount();
		void getParameter(unsigned int index, TemplateParameter* dest);
		void setParameter(unsigned int index, int value);

		void setCasingThickness(float t, unsigned int index);
		float getCasingThickness(unsigned int index);

		void setOutermostCasingShape(enum GeometricShape s);
		enum GeometricShape getCasingShape(unsigned int index);
		enum GeometricShape getOutermostCasingShape();
		bool hasSquareCasing();

		void setCasingColor(GlassColor* gc, unsigned int index);
		void setOutermostCasingColor(GlassColor* gc);
		GlassColor* getCasingColor(unsigned int index);
		GlassColor* getOutermostCasingColor();

		void addCasing(enum GeometricShape s);
		void removeCasing();
		unsigned int getCasingCount(); 

		void setTwist(int t);
		int getTwist();

		PullPlan* copy() const;

		vector<SubpullTemplate> subs;

		bool hasDependencyOn(GlassColor* color);
		bool hasDependencyOn(PullPlan* pullPlan);

	private:
		enum PullTemplate::Type templateType;
		vector<Casing> casings;
		float twist;
		vector<TemplateParameter> parameters;

		// Methods
		void initializeTemplate();
		void resetSubs(bool hardReset);
		void pushNewSubpull(bool hardReset, vector<SubpullTemplate>* newSubs,
			enum GeometricShape s, Point location, float diameter);
};

PullPlan *deep_copy(const PullPlan *plan);
void deep_delete(PullPlan *plan);

#endif

