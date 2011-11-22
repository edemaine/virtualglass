
#include "pullplan.h"

PullPlan :: PullPlan(int pullTemplate, bool isBase, Color color)
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

Color PullPlan :: getColorAverage()
{
	if (this->isBase)
		return this->color;

	Color avg;
	avg.r = avg.g = avg.b = avg.a = 0.0;

	for (unsigned int i = 0; i < this->subplans.size(); ++i)
	{
		Color spColor = this->subplans[i]->getColorAverage();
		avg.r += spColor.r / this->subplans.size();
		avg.g += spColor.g / this->subplans.size();
		avg.b += spColor.b / this->subplans.size();
		avg.a += spColor.a / this->subplans.size();
	}

	return avg;
}

void PullPlan :: setTemplate(PullTemplate* newTemplate)
{
	vector<PullPlan*> newSubplans;

	// For each new subpull, see if its group exists in the current template
	for (unsigned int i = 0; i < newTemplate->subpulls.size(); ++i)
	{
		int group = newTemplate->subpulls[i].group;

		// Look for the group in the old template, copy the plan if found
		bool matchFound = false;
		for (unsigned int j = 0; j < this->pullTemplate->subpulls.size(); ++j)
		{
			if (group == this->pullTemplate->subpulls[j].group)
			{
				newSubplans.push_back(this->subplans[j]);
				matchFound = true;
				break;
			}
		}

		if (!matchFound)
		{
			Color color;
			color.r = color.g = color.b = 1.0;
			color.a = 0.0;
			switch (newTemplate->subpulls[i].shape)
			{
				// this is a memory leak
				case CIRCLE_SHAPE:
					newSubplans.push_back(new PullPlan(CIRCLE_BASE_TEMPLATE, true, color));
					break;
				case SQUARE_SHAPE:
					newSubplans.push_back(new PullPlan(SQUARE_BASE_TEMPLATE, true, color));
					break;
			}
		}
	}

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






