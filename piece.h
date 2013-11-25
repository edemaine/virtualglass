
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
		enum PieceTemplate::Type pieceTemplateType() const;
		enum PickupTemplate::Type pickupTemplateType() const;

		unsigned int pickupParameterCount() const;
		void pickupParameter(unsigned int index, TemplateParameter* dest) const;
		void setPickupParameter(unsigned int index, int value);

		Piece* copy() const;
		void set(Piece* p);

		bool hasDependencyOn(GlassColor* c) const;
		bool hasDependencyOn(Cane* p) const;

		SubpickupTemplate subpickupTemplate(unsigned int index) const;
		void setSubpickupTemplate(SubpickupTemplate t, unsigned int index);
		unsigned int subpickupCount() const;

		void setTwist(float t);
		float twist() const;

		void setSpline(Spline s);
		Spline spline() const;

		GlassColor* overlayGlassColor() const;
		GlassColor* underlayGlassColor() const;
		GlassColor* casingGlassColor() const;
		void setOverlayGlassColor(GlassColor* c);
		void setUnderlayGlassColor(GlassColor* c);
		void setCasingGlassColor(GlassColor* c);

	signals:
		void modified();

	private:
		void updateSubcanes();
		void pushNewSubcane(vector<SubpickupTemplate>* newSubs, Point3D location,
			enum PickupCaneOrientation ori, float length, float width, enum GeometricShape s);

		float twist_;
		enum PieceTemplate::Type pieceType_;
		Spline spline_;

		GlassColor* overlayGlassColor_;
		GlassColor* underlayGlassColor_;
		GlassColor* casingGlassColor_;
		vector<SubpickupTemplate> subcanes_;
		vector<TemplateParameter> pickupParameters_;
		enum PickupTemplate::Type pickupType_;

		unsigned int casingDependencyOccurrances(GlassColor* gc);
		void addCasingDependency(GlassColor* gc);
		void removeCasingDependency(GlassColor* gc);

		unsigned int subcaneDependencyOccurrances(Cane* c);
		void addSubcaneDependency(Cane* cane);
		void removeSubcaneDependency(Cane* cane);		

	private slots:
		void dependencyModified();
};

Piece *deep_copy(const Piece *);
void deep_delete(Piece *);

#endif

