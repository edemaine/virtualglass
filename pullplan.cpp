
#include "pullplan.h"

PullPlan :: PullPlan(int pullTemplate, bool isBase, Color* color)
{
	// initialize all the variables
	this->pullTemplate = new PullTemplate(pullTemplate, 0.0);
	this->color = color;
	this->isBase = isBase;
	this->twist = 0.0;
	this->libraryWidget = NULL;
	this->colorLibraryWidget = NULL;
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
	for (unsigned int i = 0; i < newTemplate->subpulls.size(); ++i)
	{
		switch (newTemplate->subpulls[i].shape)
		{
			case CIRCLE_SHAPE:
				subplans.push_back(new PullPlan(CIRCLE_BASE_TEMPLATE, true, color));
				break;
			case SQUARE_SHAPE:
				subplans.push_back(new PullPlan(SQUARE_BASE_TEMPLATE, true, color));
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






