
#ifndef PIECE_H
#define PIECE_H

#include <vector>
#include <stack>

#include <QObject>

#include "shape.h"
#include "pickuptemplate.h"
#include "subpickuptemplate.h"
#include "templateparameter.h"
#include "piecetemplate.h"
#include "pickup.h"
#include "glasscolor.h"
#include "spline.h"

using std::vector;
using std::stack;

class Piece : public QObject
{
	Q_OBJECT

	public:
		Piece(enum PieceTemplate::Type t1, enum PickupTemplate::Type t2);

		void setPieceTemplateType(enum PieceTemplate::Type t, bool force=false);
		void setPickupTemplateType(enum PickupTemplate::Type t, bool force=false);
		enum PieceTemplate::Type pieceTemplateType();
		enum PickupTemplate::Type pickupTemplateType();

		unsigned int pickupParameterCount() const;
		void pickupParameter(unsigned int index, TemplateParameter* dest) const;
		void setPickupParameter(unsigned int index, int value);

		Piece* copy() const;
		void set(Piece* p);

		bool hasDependencyOn(GlassColor* c);
		bool hasDependencyOn(Cane* p);

		SubpickupTemplate subpickupTemplate(unsigned int index) const;
		void setSubpickupTemplate(SubpickupTemplate t, unsigned int index);
		unsigned int subpickupCount() const;

		void setTwist(float t);
		float twist() const;
		float* twistPtr();

		void setSpline(Spline s);
		Spline spline() const;

		GlassColor* overlayGlassColor();
		GlassColor* underlayGlassColor();
		GlassColor* casingGlassColor();
		void setOverlayGlassColor(GlassColor* c);
		void setUnderlayGlassColor(GlassColor* c);
		void setCasingGlassColor(GlassColor* c);

	signals:
		void modified();

	private:
		float twist_;
		enum PieceTemplate::Type pieceType_;
		Spline spline_;

		GlassColor* overlayGlassColor_;
		GlassColor* underlayGlassColor_;
		GlassColor* casingGlassColor_;
		vector<SubpickupTemplate> subcanes_;
		vector<TemplateParameter> pickupParameters_;
		enum PickupTemplate::Type pickupType_;

		void updateSubcanes();
		void pushNewSubcane(vector<SubpickupTemplate>* newSubs, Point3D location,
			enum PickupCaneOrientation ori, float length, float width, enum GeometricShape s);
	
	private slots:
		void dependencyModified();
};

Piece *deep_copy(const Piece *);
void deep_delete(Piece *);

#endif

