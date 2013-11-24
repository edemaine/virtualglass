

#ifndef PICKUP_H
#define PICKUP_H

#include <vector>
#include "shape.h"
#include "pickuptemplate.h"
#include "subpickuptemplate.h"

class GlassColor;
class TemplateParameter;

using std::vector;

class Pickup
{
	friend class Piece;

	public:
		Pickup(enum PickupTemplate::Type t);
		Pickup();

		void setTemplateType(enum PickupTemplate::Type t, bool force=false);
		enum PickupTemplate::Type templateType();

		unsigned int parameterCount();
		void parameter(unsigned int index, TemplateParameter* dest);
		void setParameter(unsigned int index, int value);

		Pickup* copy() const;
		void set(Pickup* p);

		SubpickupTemplate subpickupTemplate(unsigned int index);
		void setSubpickupTemplate(SubpickupTemplate t, unsigned int index);
		unsigned int subpickupCount();

		GlassColor* overlayGlassColor();
		GlassColor* underlayGlassColor();
		GlassColor* casingGlassColor();
		void setOverlayGlassColor(GlassColor* c);
		void setUnderlayGlassColor(GlassColor* c);
		void setCasingGlassColor(GlassColor* c);

	private:
		GlassColor* overlayGlassColor_;
		GlassColor* underlayGlassColor_;
		GlassColor* casingGlassColor_;
		vector<SubpickupTemplate> subs_;
		vector<TemplateParameter> parameters_;
		enum PickupTemplate::Type type_;

		void updateSubs();
		void pushNewSubcane(vector<SubpickupTemplate>* newSubs, Point3D location,
			enum PickupCaneOrientation ori, float length, float width, enum GeometricShape s); 
};

Pickup *deep_copy(const Pickup *);
void deep_delete(Pickup *);

#endif

