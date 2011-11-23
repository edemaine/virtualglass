
#include "pullplan.h"

PullPlan :: PullPlan(int pullTemplate, bool isBase, Color* color)
{
	// initialize all the variables
	this->pullTemplate = new PullTemplate(CIRCLE_BASE_TEMPLATE, 0.0);
	this->color = color;
	this->isBase = isBase;
	this->twist = 0.0;
	this->libraryWidget = NULL;
	this->colorLibraryWidget = NULL;

	this->setTemplate(new PullTemplate(pullTemplate, 0.0));
}

void PullPlan :: setLibraryWidget(PullPlanLibraryWidget* plplw)
{
	this->libraryWidget = plplw;
}

void PullPlan :: setLibraryWidget(ColorBarLibraryWidget* cblw)
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
	vector<PullPlan*> newSubplans;

	// create the new subplans based on template
	for (unsigned int i = 0; i < newTemplate->subpulls.size(); ++i)
	{
		Color* color = new Color();
		color->r = color->g = color->b = 1.0;
		color->a = 0.0;
		switch (newTemplate->subpulls[i].shape)
		{
			case CIRCLE_SHAPE:
				newSubplans.push_back(new PullPlan(CIRCLE_BASE_TEMPLATE, true, color));
				break; 
			case SQUARE_SHAPE:
				newSubplans.push_back(new PullPlan(SQUARE_BASE_TEMPLATE, true, color));
				break; 
		}
	}

	// load these into the pull plan
	this->pullTemplate = newTemplate;
	this->subplans.clear();
	for (unsigned int i = 0; i < this->pullTemplate->subpulls.size(); ++i)
	{
		subplans.push_back(newSubplans[i]);
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






