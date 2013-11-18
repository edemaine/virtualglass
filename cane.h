

#ifndef CANE_H
#define CANE_H

#include <vector>
#include <stack>

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

class Cane
{
	public:
		Cane(enum CaneTemplate::Type t);

		void setTemplateType(enum CaneTemplate::Type t);
		enum CaneTemplate::Type templateType() const;

		void setCount(unsigned int count);
		unsigned int count();

		void setCasingThickness(float t, unsigned int index);
		float getCasingThickness(unsigned int index);

		void setOutermostCasingShape(enum GeometricShape s);
		enum GeometricShape getCasingShape(unsigned int index);
		enum GeometricShape outermostCasingShape();

		void setCasingColor(GlassColor* gc, unsigned int index);
		void setOutermostCasingColor(GlassColor* gc);
		const GlassColor* getCasingColor(unsigned int index);
		const GlassColor* outermostCasingColor();

		float twist();
		void setTwist(float t);
		float* twistPtr();

		void addCasing(enum GeometricShape s);
		void removeCasing();
		unsigned int casingCount(); 
		bool hasMinimumCasingCount();

		Cane* copy() const;

		SubcaneTemplate getSubcaneTemplate(unsigned int index);
		void setSubcaneTemplate(SubcaneTemplate t, unsigned int index);	
		void addSubcaneTemplate(SubcaneTemplate t);
		void removeSubcaneTemplate(unsigned int index);
		unsigned int subpullCount();	

		bool hasDependencyOn(GlassColor* color);
		bool hasDependencyOn(Cane* cane);

		void undo();
		void redo();
		bool canUndo();
		bool canRedo();
		void saveState();

	private:
		struct State
		{
			enum CaneTemplate::Type type;
			vector<Casing> casings;
			unsigned int count;
			float twist;
			vector<SubcaneTemplate> subs;
		};	

		stack<struct State> undoStack;
		stack<struct State> redoStack;
		struct State state;

		// Methods
		void initializeTemplate();
		void resetSubs(bool hardReset);
		void pushNewSubpull(bool hardReset, vector<SubcaneTemplate>* newSubs,
			enum GeometricShape s, Point2D location, float diameter);

};

Cane *deep_copy(const Cane *cane);
void deep_delete(Cane *cane);

#endif

