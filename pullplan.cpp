
#include "pullplan.h"

PullPlan :: PullPlan(int pullTemplate, Color* color)
{
	// initialize all the variables
	this->pullTemplate = new PullTemplate(pullTemplate);
	this->color = color;
	this->twist = 0.0;
	this->libraryWidget = NULL;
	this->colorLibraryWidget = NULL;
}

bool PullPlan :: hasDependencyOn(PullPlan* plan)
{
	if (this == plan)
		return true;
	if (this->pullTemplate->isBase())
		return false;

        bool childrenAreDependent = false;
        for (unsigned int i = 0; i < subplans.size(); ++i)
        {
                if (subplans[i]->hasDependencyOn(plan))
                {
                        childrenAreDependent = true;
                        break;
                }
        }	

	return childrenAreDependent;
}

bool PullPlan :: hasDependencyOn(Color* color)
{
	if (this->color == color) 
		return true;
	if (this->pullTemplate->isBase())
		return false;

	bool childrenAreDependent = false;
	for (unsigned int i = 0; i < subplans.size(); ++i)
	{
		if (subplans[i]->hasDependencyOn(color))
		{
			childrenAreDependent = true;
			break;
		}
	}
	
	return childrenAreDependent;
}

void PullPlan :: setLibraryWidget(PullPlanLibraryWidget* plplw)
{
	this->libraryWidget = plplw;
}

void PullPlan :: setColorLibraryWidget(ColorBarLibraryWidget* cblw)
{
	this->colorLibraryWidget = cblw;
}

PullPlanLibraryWidget* PullPlan :: getLibraryWidget()
{
	return this->libraryWidget;
}

ColorBarLibraryWidget* PullPlan :: getColorLibraryWidget()
{
	return this->colorLibraryWidget;
}

void PullPlan :: setTemplate(PullTemplate* newTemplate)
{
	Color* color = new Color();
	color->r = color->g = color->b = 1.0;
	color->a = 0.0;

	this->color = color;

	// create the new subplans based on template
	this->pullTemplate = newTemplate;
	this->subplans.clear();
	for (unsigned int i = 0; i < newTemplate->subtemps.size(); ++i)
	{
		switch (newTemplate->subtemps[i].shape)
		{
			case CIRCLE_SHAPE:
				subplans.push_back(new PullPlan(CIRCLE_BASE_PULL_TEMPLATE, color));
				break;
			case SQUARE_SHAPE:
				subplans.push_back(new PullPlan(SQUARE_BASE_PULL_TEMPLATE, color));
				break;
		}
	}
}

PullTemplate* PullPlan :: getTemplate()
{
	return this->pullTemplate;
}

const QPixmap* PullPlan :: getEditorPixmap()
{
	return &(this->editorPixmap);
}

void PullPlan :: updatePixmap(QPixmap editorPixmap)
{
	this->editorPixmap = editorPixmap;
}






