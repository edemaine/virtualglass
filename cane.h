

#ifndef CANE_H
#define CANE_H

#include <vector>
#include <stack>

#include <QObject>

#include "canetemplate.h"
#include "shape.h"
#include "primitives.h"
#include "subcanetemplate.h"
#include "casing.h"
#include "templateparameter.h"

using std::vector;
using std::stack;

class Cane;
class GlassColor;

class Cane : public QObject
{
	Q_OBJECT

	public:
		Cane(enum CaneTemplate::Type t);

		void setTemplateType(enum CaneTemplate::Type t);
		enum CaneTemplate::Type templateType() const;

		void setCount(unsigned int count);
		unsigned int count();

		void setCasingThickness(float t, unsigned int index);
		float casingThickness(unsigned int index);

		void setOutermostCasingShape(enum GeometricShape s);
		enum GeometricShape casingShape(unsigned int index);
		enum GeometricShape outermostCasingShape();

		void setCasingColor(GlassColor* gc, unsigned int index);
		void setOutermostCasingColor(GlassColor* gc);
		const GlassColor* casingColor(unsigned int index);
		const GlassColor* outermostCasingColor();

		float twist();
		void setTwist(float t);
		float* twistPtr();

		void addCasing(enum GeometricShape s);
		void removeCasing();
		unsigned int casingCount(); 
		bool hasMinimumCasingCount();

		Cane* copy() const;
		void set(Cane* c);

		SubcaneTemplate subcaneTemplate(unsigned int index);
		void setSubcaneTemplate(SubcaneTemplate t, unsigned int index);	
		void addSubcaneTemplate(SubcaneTemplate t);
		void removeSubcaneTemplate(unsigned int index);
		unsigned int subpullCount();	

		bool hasDependencyOn(GlassColor* color);
		bool hasDependencyOn(Cane* cane);

	signals:
		void modified();

	private:
		void initializeTemplate();
		void resetSubs(bool hardReset);
		void pushNewSubpull(bool hardReset, vector<SubcaneTemplate>* newSubs,
			enum GeometricShape s, Point2D location, float diameter);

		enum CaneTemplate::Type type_;
		vector<Casing> casings_;
		unsigned int count_;
		float twist_;
		vector<SubcaneTemplate> subcanes_;

	private slots:
		void dependencyModified();
};

Cane *deep_copy(const Cane *cane);
void deep_delete(Cane *cane);

#endif

