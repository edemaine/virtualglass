
#include "coloreditorviewwidget.h"

ColorEditorViewWidget :: ColorEditorViewWidget(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	QGridLayout* palette  = new QGridLayout(this);
	palette->addWidget(new QLabel("Color 1"), 0, 0);	
	palette->addWidget(new QLabel("Color 2"), 0, 1);	
	palette->addWidget(new QLabel("Color 3"), 1, 0);	
	palette->addWidget(new QLabel("Color 4"), 1, 1);	

	this->setLayout(palette);

	this->plan = plan;
}

void ColorEditorViewWidget :: setPullPlan(PullPlan* plan)
{
	this->plan = plan;
}



