#include "pullplancustomizewidget.h"

PullPlanCustomizeWidget::PullPlanCustomizeWidget(PullPlan* plan, QWidget *parent) : QWidget(parent)
{
	this->viewWidget = new PullPlanCustomizeViewWidget(plan, this);

	setupLayout();
	setupConnections();
}

void PullPlanCustomizeWidget :: setupLayout()
{
		// Setup the editor layout
		QHBoxLayout* pageLayout = new QHBoxLayout(this);
		this->setLayout(pageLayout);
		QVBoxLayout* editorLayout = new QVBoxLayout(this);
		pageLayout->addLayout(editorLayout);

		editorLayout->addWidget(viewWidget, 0);

/*		QHBoxLayout* pullTemplateShapeLayout = new QHBoxLayout(this);
		QLabel* casingLabel = new QLabel("Casing:");
		QCheckBox* circleCheckBox = new QCheckBox("Circle");
		QCheckBox* squareCheckBox = new QCheckBox("Square");
	addCasingButton = new QPushButton("Add Casing");
		shapeButtonGroup = new QButtonGroup();
		shapeButtonGroup->addButton(circleCheckBox, 1);
		shapeButtonGroup->addButton(squareCheckBox, 2);
		pullTemplateShapeLayout->addWidget(casingLabel);
		pullTemplateShapeLayout->addWidget(circleCheckBox);
		pullTemplateShapeLayout->addWidget(squareCheckBox);
		pullTemplateShapeLayout->addWidget(addCasingButton);
		editorLayout->addLayout(pullTemplateShapeLayout, 0);*/

	QHBoxLayout* windowControlsLayout = new QHBoxLayout(this);
	confirmChangesButton = new QPushButton("Confirm changes");
	cancelButton = new QPushButton("Cancel");
	windowControlsLayout->addWidget(confirmChangesButton);
	windowControlsLayout->addWidget(cancelButton);
	editorLayout->addLayout(windowControlsLayout, 0);
}

void PullPlanCustomizeWidget :: mousePressEvent(QMouseEvent* event)
{
/*	PullTemplateLibraryWidget* ptlw = dynamic_cast<PullTemplateLibraryWidget*>(childAt(event->pos()));

	if (ptlw != NULL)
		{
				plan->setTemplateType(ptlw->getPullTemplateType());
		emit someDataChanged();
		}*/
}

void PullPlanCustomizeWidget :: setupConnections()
{
	connect(cancelButton, SIGNAL(pressed()), this, SLOT(cancelButtonPressed()));
/*	connect(fillRuleButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(fillRuleButtonGroupChanged(int)));
	connect(addCasingButton, SIGNAL(pressed()), this, SLOT(addCasingButtonPressed()));
		connect(shapeButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(shapeButtonGroupChanged(int)));
		connect(casingThicknessSlider, SIGNAL(valueChanged(int)),
				this, SLOT(casingThicknessSliderChanged(int)));
		connect(casingThicknessSpin, SIGNAL(valueChanged(int)),
				this, SLOT(casingThicknessSpinChanged(int)));
		connect(twistSlider, SIGNAL(valueChanged(int)), this, SLOT(twistSliderChanged(int)));
		connect(twistSpin, SIGNAL(valueChanged(int)), this, SLOT(twistSpinChanged(int)));

	for (unsigned int i = 0; i < paramSpins.size(); ++i)
	{
		connect(paramSpins[i], SIGNAL(valueChanged(int)), this, SLOT(paramSpinChanged(int)));
	}

	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(viewWidget, SIGNAL(someDataChanged()), this, SLOT(viewWidgetDataChanged()));*/
}

/*PullPlan PullPlanCustomizeWidget :: getPlan()
{
	return viewWidget.getPlan();
}*/

void PullPlanCustomizeWidget :: setPullPlan(PullPlan* plan)
{
	viewWidget->setPullPlan(plan);
}

void PullPlanCustomizeWidget :: openWindow(PullPlan* plan)
{
	viewWidget->setPullPlan(plan);
	this->show();
}

void PullPlanCustomizeWidget :: cancelButtonPressed()
{
	viewWidget->revertAndClose();
	this->hide();
}
