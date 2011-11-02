
#include "pullplan.h"

PullPlan :: PullPlan(int pullTemplate, bool isBase, Color color)
{
	setTemplate(new PullTemplate(pullTemplate, 0.0));
	this->color = color;
	this->isBase = isBase;
	this->twist = 0.0;
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

void PullPlan :: setTemplate(PullTemplate* pt)
{
	this->pullTemplate = pt;
	this->subplans.clear();

	// initialize the pull plan's subplans to be something boring and base
	for (unsigned int i = 0; i < pt->subpulls.size(); ++i)
	{
		// Set color based on group, only support for 3 unique groups;
		// Additional groups all show up grey
		Color color;
		color.r = color.g = color.b = 1.0;
		color.a = 0.4;
		switch (pt->subpulls[i].group)
		{
			case 0:
				color.r = color.g = 0.4;
				break;
			case 1:
				color.r = color.b = 0.4;
				break;
			case 2:
				color.g = color.b = 0.4;
				break;
			default:
				break;
		}

		switch (pt->subpulls[i].shape)
		{
			// this is a memory leak
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






