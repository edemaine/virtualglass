

#ifndef PICKUPPLAN_H
#define PICKUPPLAN_H

#include <vector>
#include "shape.h"
#include "pickuptemplate.h"
#include "subpickuptemplate.h"

class GlassColor;
class TemplateParameter;

using std::vector;

class PickupPlan
{
	friend class Piece;

	public:
		PickupPlan(enum PickupTemplate::Type t);
		PickupPlan();

		void setTemplateType(enum PickupTemplate::Type t, bool force=false);
		enum PickupTemplate::Type templateType();

		unsigned int parameterCount();
		void getParameter(unsigned int index, TemplateParameter* dest);
		void setParameter(unsigned int index, int value);

		PickupPlan* copy() const;

		SubpickupTemplate getSubpickupTemplate(unsigned int index);
		void setSubpickupTemplate(SubpickupTemplate t, unsigned int index);
		unsigned int subpickupCount();

		GlassColor* overlayGlassColor();
		GlassColor* underlayGlassColor();
		GlassColor* casingGlassColor();
		void setOverlayGlassColor(GlassColor* c);
		void setUnderlayGlassColor(GlassColor* c);
		void setCasingGlassColor(GlassColor* c);

	private:
		struct State
		{
			GlassColor* overlayGlassColor;
			GlassColor* underlayGlassColor;
			GlassColor* casingGlassColor;
			vector<SubpickupTemplate> subs;
			vector<TemplateParameter> parameters;
		};	

		struct State state;

		void updateSubs();
		void pushNewSubplan(vector<SubpickupTemplate>* newSubs, Point3D location,
			enum PickupCaneOrientation ori, float length, float width, enum GeometricShape s); 
		enum PickupTemplate::Type type;
};

PickupPlan *deep_copy(const PickupPlan *);
void deep_delete(PickupPlan *);

#endif

