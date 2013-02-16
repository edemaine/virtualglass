

#ifndef PULLPLAN_H
#define PULLPLAN_H

#include <vector>
#include "pulltemplate.h"
#include "shape.h"
#include "primitives.h"
#include "subpulltemplate.h"
#include "casing.h"
#include "templateparameter.h"

using std::vector;

class PullPlan;
class GlassColor;

class PullPlan
{
	public:
		PullPlan(enum PullTemplate::Type t);

		void setTemplateType(enum PullTemplate::Type t, bool force=false);
		enum PullTemplate::Type getTemplateType();

		bool hasParameter();
		unsigned int getCount();
		void setCount(unsigned int count);

		void setCasingThickness(float t, unsigned int index);
		float getCasingThickness(unsigned int index);

		void setOutermostCasingShape(enum GeometricShape s);
		enum GeometricShape getCasingShape(unsigned int index);
		enum GeometricShape getOutermostCasingShape();

		void setCasingColor(GlassColor* gc, unsigned int index);
		void setOutermostCasingColor(GlassColor* gc);
		GlassColor* getCasingColor(unsigned int index);
		GlassColor* getOutermostCasingColor();

		float twist;

		void addCasing(enum GeometricShape s);
		void removeCasing();
		unsigned int getCasingCount(); 
		bool hasMinimumCasingCount();

		PullPlan* copy() const;

		vector<SubpullTemplate> subs;

		bool hasDependencyOn(GlassColor* color);
		bool hasDependencyOn(PullPlan* pullPlan);

	private:
		enum PullTemplate::Type templateType;
		vector<Casing> casings;
		unsigned int count;

		// Methods
		void initializeTemplate();
		void resetSubs(bool hardReset);
		void pushNewSubpull(bool hardReset, vector<SubpullTemplate>* newSubs,
			enum GeometricShape s, Point location, float diameter);
};

PullPlan *deep_copy(const PullPlan *plan);
void deep_delete(PullPlan *plan);

#endif

